
#include "sound_font_player.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/audio_stream_generator.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>


#define TSF_IMPLEMENTATION
#include "tsf/tsf.h"

using namespace godot;

SoundFontPlayer::SoundFontPlayer() {
    Ref<AudioStreamGenerator> genstream;
    genstream.instantiate();
    // Set reasonable snappy default latency
    genstream->set_buffer_length(0.1 /* seconds */);
    set_stream(genstream);
    generator = nullptr;
    // Start with quiet gain, for mixing 4 voices at 0 dB
    // Actual required gain depends on sf2 levels.
    gain = -12.0f;
    max_voices = 32;
    set_autoplay(true);
    time = 0.0;
    // Default goal is to have 80% of buffer available (so use 20% of 100 ms for buffer, about 20 ms of buffer delay)
    goal_available_ratio = 0.8f;
    max_samples_available = 0;
    process_count = 0;
}

SoundFontPlayer::~SoundFontPlayer() {
    set_stream(nullptr);
    if (generator) {
        tsf_close(generator);
        generator = nullptr;
    }
}

void SoundFontPlayer::setup_generator() {
    Ref<AudioStreamGenerator> stream = get_stream();
    if (generator && stream.is_valid()) {
        int mix_rate = stream->get_mix_rate();
        tsf_set_output(generator, TSF_STEREO_INTERLEAVED, mix_rate, gain);
        tsf_set_max_voices(generator, std::max(max_voices, 1));
    }
}

void SoundFontPlayer::set_soundfont(Ref<SoundFont> p_soundfont) {
    if (generator) {
        tsf_close(generator);
        generator = nullptr;
    }
    soundfont = p_soundfont;
    if (soundfont.is_valid() && soundfont->get_data().size()) {
        generator = tsf_load_memory(soundfont->get_data().ptr(), soundfont->get_data().size());
        if (!generator) {
            UtilityFunctions::printerr("Error parsing SF2 resource inside SoundFontPlayer");
        }
    }
    setup_generator();
}

Ref<SoundFont> SoundFontPlayer::get_soundfont() {
    return soundfont;
}

void SoundFontPlayer::set_gain(float p_gain) {
    gain = p_gain;
    setup_generator();
}

float SoundFontPlayer::get_gain() {
    return gain;
}

void SoundFontPlayer::set_max_voices(int p_max_voices) {
    max_voices = p_max_voices;
    setup_generator();
}

int SoundFontPlayer::get_max_voices() {
    return max_voices;
}

void SoundFontPlayer::set_goal_available_ratio(float p_goal_available_ratio) {
    goal_available_ratio = p_goal_available_ratio;
}

float SoundFontPlayer::get_goal_available_ratio() {
    return goal_available_ratio;
}

double SoundFontPlayer::get_time() {
    return time;
}

int SoundFontPlayer::get_presetindex(int bank, int preset_number) {
    if (!generator) {
        return -1;
    }
    return tsf_get_presetindex(generator, bank, preset_number);
}

int SoundFontPlayer::get_presetcount() {
    if (!generator) {
        return -1;
    }
    return tsf_get_presetcount(generator);
}

String SoundFontPlayer::get_presetname(int preset_index) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont loaded in SoundFontPlayer");
        return String("");
    }
    return String(tsf_get_presetname(generator, preset_index));
}

int SoundFontPlayer::get_active_voice_count() {
    if (!generator) {
        return -1;
    }
    return tsf_active_voice_count(generator);
}

void SoundFontPlayer::note_off(double time, int preset_index, int key) {
    events.push_back(Event(time, EventType::NOTE_OFF, -1, preset_index, key, 0.0f));
}

void SoundFontPlayer::note_on(double time, int preset_index, int key, float velocity) {
    events.push_back(Event(time, EventType::NOTE_ON, -1, preset_index, key, velocity));
}

void SoundFontPlayer::note_off_all(double time) {
    events.push_back(Event(time, EventType::NOTE_OFF_ALL, -1, 0, 0, 0.0f));
}

void SoundFontPlayer::do_event(const Event &event) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont loaded in SoundFontPlayer");
        return;
    }
    switch (event.event_type) {
        case EventType::NOTE_OFF:
            tsf_note_off(generator, event.preset_index, event.key);
            break;
        case EventType::NOTE_ON:
            tsf_note_on(generator, event.preset_index, event.key, event.velocity);
            break;
        case EventType::NOTE_OFF_ALL:
            tsf_note_off_all(generator);
            break;
    }
}

PackedVector2Array SoundFontPlayer::render(int samples) {
    PackedVector2Array result{};
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont loaded in SoundFontPlayer");
        return result;
    }
    result.resize(samples);
    tsf_render_float(generator, reinterpret_cast<float*>(result.ptrw()), samples);
    return result;
}

void SoundFontPlayer::_physics_process() {
    double delta = 1.0 / Engine::get_singleton()->get_physics_ticks_per_second();
    if (Engine::get_singleton()->is_editor_hint()) {
        // In editor, don't play SoundFont audio data
        // Also reset max_samples_available in case the buffer settings are modified
        max_samples_available = 0;
        return;
    }
    Ref<AudioStreamGenerator> stream = get_stream();
    if (!stream.is_valid()) {
        // stream needs to be valid AudioStreamGenerator to generate anything
        max_samples_available = 0;
        return;
    }
    Ref<AudioStreamGeneratorPlayback> playback = get_stream_playback();
    if (!playback.is_valid()) {
        // If there is no playback, stop
        max_samples_available = 0;
        return;
    }
    float mix_rate = stream->get_mix_rate();
    float buffer_length = stream->get_buffer_length();
    int available = playback->get_frames_available();
    // Just set max_samples_available on first call
    if (max_samples_available == 0) {
        max_samples_available = std::max(max_samples_available, available);
    }
    int goal_available = static_cast<int>(max_samples_available * goal_available_ratio);
    int ideal_samples = static_cast<int>(delta * mix_rate);
    // If buffer is too empty, use large samples to help fill it up
    // Otherwise use small samples to let it drain
    // Push number of samples to correct part of gap to goal_available
    int samples = ideal_samples + (available - goal_available) / 32;
    // If buffer is almost full, never overfill (cut down samples with min)
    samples = std::max(std::min(samples, available), 0);
    if (!samples) {
        // Nothing to render
        return;
    }
    //UtilityFunctions::print("SoundFontPlayer samples=", samples, " events=", events.size());
    int rendered_samples = 0;
    int loop_count = 0;
    int loop_limit = 256;
    while (rendered_samples < samples) {
        // First make sure we are not stuck in infinite loop
        loop_count++;
        if (loop_count > loop_limit) {
            UtilityFunctions::print("SoundFontPlayer too many iteration in main processing loop, aborting");
            break;
        }
        // Events must be sorted by time (earliest first)
        // First look for any events that were scheduled in past and do them
        for (auto &event : events) {
            if (event.time <= time) {
                do_event(event);
            }
        }
        // Now remove events in the past
        int position = 0;
        while (position < events.size() && events[position].time <= time) {
            position++;
        }
        for (int i = 0; i < position; i++) {
            events.remove_at(0);
        }
        // Now find timestep to render too
        int render_size = samples - rendered_samples;
        if (events.size() > 0) {
            // How long until next event
            double delta_event_time = events[0].time - time;
            // Render shorter than remaining buffer if the event intersects the current buffer
            // +1 is to ensure we are making progress, even if floating point result rounds down
            render_size = std::min(render_size, static_cast<int>(delta_event_time * mix_rate) + 1);
        }
        time += render_size / mix_rate;
        playback->push_buffer(render(render_size));
        rendered_samples += render_size;
    }
}

void SoundFontPlayer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont"), &SoundFontPlayer::set_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont"), &SoundFontPlayer::get_soundfont);
    ClassDB::bind_method(D_METHOD("set_gain", "gain"), &SoundFontPlayer::set_gain);
    ClassDB::bind_method(D_METHOD("get_gain"), &SoundFontPlayer::get_gain);
    ClassDB::bind_method(D_METHOD("set_max_voices", "max_voices"), &SoundFontPlayer::set_max_voices);
    ClassDB::bind_method(D_METHOD("get_max_voices"), &SoundFontPlayer::get_max_voices);
    ClassDB::bind_method(D_METHOD("set_goal_available_ratio", "goal_available_ratio"), &SoundFontPlayer::set_goal_available_ratio);
    ClassDB::bind_method(D_METHOD("get_goal_available_ratio"), &SoundFontPlayer::get_goal_available_ratio);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "SoundFont"), "set_soundfont", "get_soundfont");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PROPERTY_HINT_RANGE, "-48.0,12.0,0.1,suffix:dB"), "set_gain", "get_gain");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_voices", PROPERTY_HINT_RANGE, "1,256,1"), "set_max_voices", "get_max_voices");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "goal_available_ratio", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_goal_available_ratio", "get_goal_available_ratio");
    ClassDB::bind_method(D_METHOD("get_time"), &SoundFontPlayer::get_time);
    ClassDB::bind_method(D_METHOD("get_presetindex", "bank", "preset_number"), &SoundFontPlayer::get_presetindex);
    ClassDB::bind_method(D_METHOD("get_presetcount"), &SoundFontPlayer::get_presetcount);
    ClassDB::bind_method(D_METHOD("get_presetname", "preset_index"), &SoundFontPlayer::get_presetname);
    ClassDB::bind_method(D_METHOD("get_active_voice_count"), &SoundFontPlayer::get_active_voice_count);
    ClassDB::bind_method(D_METHOD("note_on", "time", "preset_index", "key", "velocity"), &SoundFontPlayer::note_on);
    ClassDB::bind_method(D_METHOD("note_off", "time", "preset_index", "key"), &SoundFontPlayer::note_off);
    ClassDB::bind_method(D_METHOD("note_off_all", "time"), &SoundFontPlayer::note_off_all);
    ClassDB::bind_method(D_METHOD("physics_process"), &SoundFontPlayer::_physics_process);
}
