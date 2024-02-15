#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#define TSF_IMPLEMENTATION
#include "tsf/tsf.h"

using namespace godot;

void SoundFont::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_data", "data"), &SoundFont::set_data);
    ClassDB::bind_method(D_METHOD("get_data"), &SoundFont::get_data);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data"), "set_data", "get_data");

}

void SoundFont::set_data(const PackedByteArray &p_data) {
    sfdata.resize(p_data.size());
    std::memcpy(sfdata.ptrw(), p_data.ptr(), p_data.size());
}

PackedByteArray SoundFont::get_data() const {
    PackedByteArray result;
    result.resize(sfdata.size());
    std::memcpy(result.ptrw(), sfdata.ptr(), sfdata.size());
    return result;
}

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

void SoundFontPlayer::_physics_process() {
    double delta = 1.0/60.0;
    if (Engine::get_singleton()->is_editor_hint()) {
        // In editor, don't play SoundFont audio data
        // Also reset max_samples_available in case buffer settings modified
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
    playback->push_buffer(render(samples));
}

void SoundFontPlayer::set_soundfont(Ref<SoundFont> p_soundfont) {
    if (generator) {
        tsf_close(generator);
        generator = nullptr;
    }
    soundfont = p_soundfont;
    if (soundfont.is_valid() && soundfont->get_data().size()) {
        UtilityFunctions::print("SoundFontPlayer set_soundfont called, size=", soundfont->get_data().size());
        generator = tsf_load_memory(soundfont->get_data().ptr(), soundfont->get_data().size());
        if (!generator) {
            UtilityFunctions::printerr("Error parsing SF2 resource inside SoundFontPlayer");
        }
    } else {
        UtilityFunctions::print("SoundFontPlayer set_soundfont called (empty soundfont)");
    }
    setup_generator();
}

Ref<SoundFont> SoundFontPlayer::get_soundfont() const {
    return soundfont;
}

void SoundFontPlayer::set_gain(float p_gain) {
    gain = p_gain;
    setup_generator();
}

float SoundFontPlayer::get_gain() const {
    return gain;
}

void SoundFontPlayer::set_max_voices(int p_max_voices) {
    max_voices = p_max_voices;
    setup_generator();
}

int SoundFontPlayer::get_max_voices() const {
    return max_voices;
}

void SoundFontPlayer::set_goal_available_ratio(float p_goal_available_ratio) {
    goal_available_ratio = p_goal_available_ratio;
}

float SoundFontPlayer::get_goal_available_ratio() const {
    return goal_available_ratio;
}

int SoundFontPlayer::get_presetindex(int bank, int preset_number) const {
    if (!generator) {
        return -1;
    }
    return tsf_get_presetindex(generator, bank, preset_number);
}

int SoundFontPlayer::get_presetcount() const {
    if (!generator) {
        return -1;
    }
    return tsf_get_presetcount(generator);
}

String SoundFontPlayer::get_presetname(int preset_index) const {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontPlayer");
        return String("");
    }
    return String(tsf_get_presetname(generator, preset_index));
}

String SoundFontPlayer::bank_get_presetname(int bank, int preset_number) const {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontPlayer");
        return String("");
    }
    return String(tsf_bank_get_presetname(generator, bank, preset_number));
}

void SoundFontPlayer::note_on(int preset_index, int key, float velocity) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontPlayer");
        return;
    }
    if (!tsf_note_on(generator, preset_index, key, velocity)) {
        UtilityFunctions::printerr("Could not allocate voice for note");
        return;
    }
}

void SoundFontPlayer::bank_note_on(int bank, int preset_number, int key, float velocity) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontPlayer");
        return;
    }
    if (!tsf_bank_note_on(generator, bank, preset_number, key, velocity)) {
        UtilityFunctions::printerr("Could not allocate voice for note");
        return;
    }
}

void SoundFontPlayer::note_off(int preset_index, int key) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontPlayer");
        return;
    }
    tsf_note_off(generator, preset_index, key);
}

void SoundFontPlayer::bank_note_off(int bank, int preset_number, int key) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontPlayer");
        return;
    }
    if (!tsf_bank_note_off(generator, bank, preset_number, key)) {
        UtilityFunctions::printerr("Preset not found");
        return;
    }
}

PackedVector2Array SoundFontPlayer::render(int samples) {
    PackedVector2Array result{};
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontPlayer");
        return result;
    }
    result.resize(samples);
    tsf_render_float(generator, reinterpret_cast<float*>(result.ptrw()), samples);
    return result;
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
    ClassDB::bind_method(D_METHOD("get_presetindex", "bank", "preset_number"), &SoundFontPlayer::get_presetindex);
    ClassDB::bind_method(D_METHOD("get_presetcount"), &SoundFontPlayer::get_presetcount);
    ClassDB::bind_method(D_METHOD("get_presetname", "preset_index"), &SoundFontPlayer::get_presetname);
    ClassDB::bind_method(D_METHOD("bank_get_presetname", "bank", "preset_number"), &SoundFontPlayer::bank_get_presetname);
    ClassDB::bind_method(D_METHOD("note_on", "preset_index", "key", "velocity"), &SoundFontPlayer::note_on);
    ClassDB::bind_method(D_METHOD("bank_note_on", "bank", "preset_number", "key", "velocity"), &SoundFontPlayer::bank_note_on);
    ClassDB::bind_method(D_METHOD("note_off", "preset_index", "key"), &SoundFontPlayer::note_off);
    ClassDB::bind_method(D_METHOD("bank_note_off", "bank", "preset_number", "key"), &SoundFontPlayer::bank_note_off);
    ClassDB::bind_method(D_METHOD("physics_process"), &SoundFontPlayer::_physics_process);
}
