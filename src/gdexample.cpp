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

SFPlayer::SFPlayer() {
    genstream.instantiate();
    set_stream(genstream);
    generator = nullptr;
    gain = -12.0f;
    max_voices = 32;
    set_autoplay(true);
}

SFPlayer::~SFPlayer() {
    set_stream(nullptr);
    genstream.unref();
    if (generator) {
        tsf_close(generator);
        generator = nullptr;
    }
}

void SFPlayer::setup_generator() {
    if (generator && genstream.is_valid()) {
        UtilityFunctions::print("SFPlayer setup_generator setting mix_rate=", genstream->get_mix_rate(), " gain=", gain, " max_voices=", max_voices);
        int mix_rate = genstream->get_mix_rate();
        tsf_set_output(generator, TSF_STEREO_INTERLEAVED, mix_rate, gain);
        tsf_set_max_voices(generator, std::min(max_voices, 1));
    }
}

void SFPlayer::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        // In editor, don't play SoundFont audio data
        return;
    }
    if (!genstream.is_valid()) {
        // stream needs to be valid to generate anything
        UtilityFunctions::print("SFPlayer _process invalid genstream");
        return;
    }
    Ref<AudioStreamGeneratorPlayback> playback = get_stream_playback();
    if (!playback.is_valid()) {
        UtilityFunctions::print("SFPlayer _process invalid playback stream");
        return;
    }
    int available = playback->get_frames_available();
    UtilityFunctions::print("SFPlayer _process delta=", delta, " available=", available);
}

void SFPlayer::set_soundfont(Ref<SoundFont> p_soundfont) {
    if (generator) {
        tsf_close(generator);
        generator = nullptr;
    }
    soundfont = p_soundfont;
    if (soundfont.is_valid() && soundfont->get_data().size()) {
        UtilityFunctions::print("SFPlayer set_soundfont called, size=", soundfont->get_data().size());
        generator = tsf_load_memory(soundfont->get_data().ptr(), soundfont->get_data().size());
        if (!generator) {
            UtilityFunctions::printerr("Error parsing SF2 resource inside SFPlayer");
        }
    } else {
        UtilityFunctions::print("SFPlayer set_soundfont called (empty soundfont)");
    }
    setup_generator();
}

Ref<SoundFont> SFPlayer::get_soundfont() const {
    return soundfont;
}

void SFPlayer::set_gain(float p_gain) {
    gain = p_gain;
    setup_generator();
}

float SFPlayer::get_gain() const {
    return gain;
}

void SFPlayer::set_max_voices(int p_max_voices) {
    max_voices = p_max_voices;
    setup_generator();
}

int SFPlayer::get_max_voices() const {
    return max_voices;
}

int SFPlayer::get_presetindex(int bank, int preset_number) const {
    if (!generator) {
        return -1;
    }
    return tsf_get_presetindex(generator, bank, preset_number);
}

int SFPlayer::get_presetcount() const {
    if (!generator) {
        return -1;
    }
    return tsf_get_presetcount(generator);
}

String SFPlayer::get_presetname(int preset_index) const {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SFPlayer");
        return String("");
    }
    return String(tsf_get_presetname(generator, preset_index));
}

String SFPlayer::bank_get_presetname(int bank, int preset_number) const {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SFPlayer");
        return String("");
    }
    return String(tsf_bank_get_presetname(generator, bank, preset_number));
}

void SFPlayer::note_on(int preset_index, int key, float velocity) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SFPlayer");
        return;
    }
    if (!tsf_note_on(generator, preset_index, key, velocity)) {
        UtilityFunctions::printerr("Could not allocate voice for note");
        return;
    }
}

void SFPlayer::bank_note_on(int bank, int preset_number, int key, float velocity) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SFPlayer");
        return;
    }
    if (!tsf_bank_note_on(generator, bank, preset_number, key, velocity)) {
        UtilityFunctions::printerr("Could not allocate voice for note");
        return;
    }
}

void SFPlayer::note_off(int preset_index, int key) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SFPlayer");
        return;
    }
    tsf_note_off(generator, preset_index, key);
}

void SFPlayer::bank_note_off(int bank, int preset_number, int key) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SFPlayer");
        return;
    }
    if (!tsf_bank_note_off(generator, bank, preset_number, key)) {
        UtilityFunctions::printerr("Preset not found");
        return;
    }
}

PackedVector2Array SFPlayer::render(int samples) {
    PackedVector2Array result{};
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SFPlayer");
        return result;
    }
    result.resize(samples);
    tsf_render_float(generator, reinterpret_cast<float*>(result.ptrw()), samples);
    return result;
}

void SFPlayer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont"), &SFPlayer::set_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont"), &SFPlayer::get_soundfont);
    ClassDB::bind_method(D_METHOD("set_gain", "gain"), &SFPlayer::set_gain);
    ClassDB::bind_method(D_METHOD("get_gain"), &SFPlayer::get_gain);
    ClassDB::bind_method(D_METHOD("set_max_voices", "max_voices"), &SFPlayer::set_max_voices);
    ClassDB::bind_method(D_METHOD("get_max_voices"), &SFPlayer::get_max_voices);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "SoundFont"), "set_soundfont", "get_soundfont");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PROPERTY_HINT_RANGE, "-48.0,12.0,0.1,suffix:dB"), "set_gain", "get_gain");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_voices", PROPERTY_HINT_RANGE, "1,256,1"), "set_max_voices", "get_max_voices");

    ClassDB::bind_method(D_METHOD("get_presetindex", "bank", "preset_number"), &SFPlayer::get_presetindex);
    ClassDB::bind_method(D_METHOD("get_presetcount"), &SFPlayer::get_presetcount);
    ClassDB::bind_method(D_METHOD("get_presetname", "preset_index"), &SFPlayer::get_presetname);
    ClassDB::bind_method(D_METHOD("bank_get_presetname", "bank", "preset_number"), &SFPlayer::bank_get_presetname);
}
