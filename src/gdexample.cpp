#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>
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

SoundFontGenerator::SoundFontGenerator() {
    // Initialize any variables here.
    time_passed = 0.0f;
    mix_rate = 44100.0f;
    stereo = true;
    gain = -12.0f;
    max_voices = 32;
    generator = nullptr;
}

SoundFontGenerator::~SoundFontGenerator() {
    if (generator) {
        tsf_close(generator);
    }
}

void SoundFontGenerator::setup_generator() {
    if (generator) {
        tsf_set_output(generator, stereo ? TSF_STEREO_INTERLEAVED : TSF_MONO, static_cast<int>(mix_rate), gain);
        tsf_set_max_voices(generator, max_voices);
    }
}

void SoundFontGenerator::set_mix_rate(float p_mix_rate) {
    mix_rate = p_mix_rate;
    setup_generator();
}

float SoundFontGenerator::get_mix_rate() const {
    return mix_rate;    
}

void SoundFontGenerator::set_soundfont(Ref<SoundFont> p_soundfont) {
    if (generator) {
        tsf_close(generator);
        generator = nullptr;
    }
    soundfont = p_soundfont;
    if (soundfont.is_valid() && soundfont->get_data().size()) {
        UtilityFunctions::print("SoundFontGenerator set_soundfont called, size=", soundfont->get_data().size());
        generator = tsf_load_memory(soundfont->get_data().ptr(), soundfont->get_data().size());
        if (!generator) {
            UtilityFunctions::printerr("Error parsing SF2 resource inside SoundFontGenerator");
        }
    } else {
        UtilityFunctions::print("SoundFontGenerator set_soundfont called (empty soundfont)");
    }
    setup_generator();
}

Ref<SoundFont> SoundFontGenerator::get_soundfont() const {
    return soundfont;
}

void SoundFontGenerator::set_stereo(bool p_stereo) {
    stereo = p_stereo;
    setup_generator();
}

bool SoundFontGenerator::get_stereo() const {
    return stereo;
}

void SoundFontGenerator::set_gain(float p_gain) {
    gain = p_gain;
    setup_generator();
}

float SoundFontGenerator::get_gain() const {
    return gain;
}

void SoundFontGenerator::set_max_voices(int p_max_voices) {
    max_voices = p_max_voices;
    setup_generator();
}

int SoundFontGenerator::get_max_voices() const {
    return max_voices;
}

void SoundFontGenerator::_process(double delta) {
    time_passed += delta;
}

int SoundFontGenerator::get_presetindex(int bank, int preset_number) const {
    if (!generator) {
        return -1;
    }
    return tsf_get_presetindex(generator, bank, preset_number);
}

int SoundFontGenerator::get_presetcount() const {
    if (!generator) {
        return -1;
    }
    return tsf_get_presetcount(generator);
}

String SoundFontGenerator::get_presetname(int preset_index) const {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontGenerator");
        return String("");
    }
    return String(tsf_get_presetname(generator, preset_index));
}

String SoundFontGenerator::bank_get_presetname(int bank, int preset_number) const {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontGenerator");
        return String("");
    }
    return String(tsf_bank_get_presetname(generator, bank, preset_number));
}

void SoundFontGenerator::note_on(int preset_index, int key, float velocity) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontGenerator");
        return;
    }
    if (!tsf_note_on(generator, preset_index, key, velocity)) {
        UtilityFunctions::printerr("Could not allocate voice for note");
        return;
    }
}

void SoundFontGenerator::bank_note_on(int bank, int preset_number, int key, float velocity) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontGenerator");
        return;
    }
    if (!tsf_bank_note_on(generator, bank, preset_number, key, velocity)) {
        UtilityFunctions::printerr("Could not allocate voice for note");
        return;
    }
}

void SoundFontGenerator::note_off(int preset_index, int key) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontGenerator");
        return;
    }
    tsf_note_off(generator, preset_index, key);
}

void SoundFontGenerator::bank_note_off(int bank, int preset_number, int key) {
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontGenerator");
        return;
    }
    if (!tsf_bank_note_off(generator, bank, preset_number, key)) {
        UtilityFunctions::printerr("Preset not found");
        return;
    }
}

PackedVector2Array SoundFontGenerator::render(int samples) {
    PackedVector2Array result{};
    if (!generator) {
        UtilityFunctions::printerr("No SoundFont generator loaded in SoundFontGenerator");
        return result;
    }
    result.resize(samples);
    if (stereo) {
        tsf_render_float(generator, reinterpret_cast<float*>(result.ptrw()), samples);
        return result;
    }
    // For mono, need to duplicate values for return value
    Vector<float> temp;
    temp.resize(samples);
    tsf_render_float(generator, temp.ptrw(), samples);
    for (int i = 0; i < samples; i++) {
        result[i] = Vector2(temp[i], temp[i]);
    }
    return result;
}

void SoundFontGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_mix_rate", "hz"), &SoundFontGenerator::set_mix_rate);
    ClassDB::bind_method(D_METHOD("get_mix_rate"), &SoundFontGenerator::get_mix_rate);
    ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont"), &SoundFontGenerator::set_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont"), &SoundFontGenerator::get_soundfont);
    ClassDB::bind_method(D_METHOD("set_stereo", "stereo"), &SoundFontGenerator::set_stereo);
    ClassDB::bind_method(D_METHOD("get_stereo"), &SoundFontGenerator::get_stereo);
    ClassDB::bind_method(D_METHOD("set_gain", "gain"), &SoundFontGenerator::set_gain);
    ClassDB::bind_method(D_METHOD("get_gain"), &SoundFontGenerator::get_gain);
    ClassDB::bind_method(D_METHOD("set_max_voices", "max_voices"), &SoundFontGenerator::set_max_voices);
    ClassDB::bind_method(D_METHOD("get_max_voices"), &SoundFontGenerator::get_max_voices);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "SoundFont"), "set_soundfont", "get_soundfont");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix_rate", PROPERTY_HINT_RANGE, "20,192000,1,suffix:Hz"), "set_mix_rate", "get_mix_rate");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "stereo"), "set_stereo", "get_stereo");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PROPERTY_HINT_RANGE, "-48.0,12.0,0.1,suffix:dB"), "set_gain", "get_gain");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_voices", PROPERTY_HINT_RANGE, "1,256,1"), "set_max_voices", "get_max_voices");

    ClassDB::bind_method(D_METHOD("get_presetindex", "bank", "preset_number"), &SoundFontGenerator::get_presetindex);
    ClassDB::bind_method(D_METHOD("get_presetcount"), &SoundFontGenerator::get_presetcount);
    ClassDB::bind_method(D_METHOD("get_presetname", "preset_index"), &SoundFontGenerator::get_presetname);
    ClassDB::bind_method(D_METHOD("bank_get_presetname", "bank", "preset_number"), &SoundFontGenerator::bank_get_presetname);
    ClassDB::bind_method(D_METHOD("note_on", "preset_index", "key", "velocity"), &SoundFontGenerator::note_on);
    ClassDB::bind_method(D_METHOD("bank_note_on", "bank", "preset_number", "key", "velocity"), &SoundFontGenerator::bank_note_on);
    ClassDB::bind_method(D_METHOD("note_off", "preset_index", "key"), &SoundFontGenerator::note_off);
    ClassDB::bind_method(D_METHOD("bank_note_off", "bank", "preset_number", "key"), &SoundFontGenerator::bank_note_off);
    ClassDB::bind_method(D_METHOD("render", "samples"), &SoundFontGenerator::render);
}

SoundFontPlayer::SoundFontPlayer() {
    gain = -12.0f;
    max_voices = 32;
    generator = nullptr;
}

SoundFontPlayer::~SoundFontPlayer() {
    if (generator) {
        tsf_close(generator);
    }
}

void SoundFontPlayer::setup_generator() {
    if (generator) {
        int mix_rate = get_mix_rate();
        tsf_set_output(generator, TSF_STEREO_INTERLEAVED, mix_rate, gain);
        tsf_set_max_voices(generator, max_voices);
    }
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

void SoundFontPlayer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont"), &SoundFontPlayer::set_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont"), &SoundFontPlayer::get_soundfont);
    ClassDB::bind_method(D_METHOD("set_gain", "gain"), &SoundFontPlayer::set_gain);
    ClassDB::bind_method(D_METHOD("get_gain"), &SoundFontPlayer::get_gain);
    ClassDB::bind_method(D_METHOD("set_max_voices", "max_voices"), &SoundFontPlayer::set_max_voices);
    ClassDB::bind_method(D_METHOD("get_max_voices"), &SoundFontPlayer::get_max_voices);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "SoundFont"), "set_soundfont", "get_soundfont");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PROPERTY_HINT_RANGE, "-48.0,12.0,0.1,suffix:dB"), "set_gain", "get_gain");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_voices", PROPERTY_HINT_RANGE, "1,256,1"), "set_max_voices", "get_max_voices");

    ClassDB::bind_method(D_METHOD("get_presetindex", "bank", "preset_number"), &SoundFontPlayer::get_presetindex);
    ClassDB::bind_method(D_METHOD("get_presetcount"), &SoundFontPlayer::get_presetcount);
    ClassDB::bind_method(D_METHOD("get_presetname", "preset_index"), &SoundFontPlayer::get_presetname);
    ClassDB::bind_method(D_METHOD("bank_get_presetname", "bank", "preset_number"), &SoundFontPlayer::bank_get_presetname);
}
