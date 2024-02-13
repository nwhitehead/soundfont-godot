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
    ClassDB::bind_method(D_METHOD("get_first"), &SoundFont::get_first);
}

int SoundFont::get_first() const {
    return sfdata[0];
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

void SoundFontGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_mix_rate", "hz"), &SoundFontGenerator::set_mix_rate);
    ClassDB::bind_method(D_METHOD("get_mix_rate"), &SoundFontGenerator::get_mix_rate);
    ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont"), &SoundFontGenerator::set_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont"), &SoundFontGenerator::get_soundfont);
    ClassDB::bind_method(D_METHOD("set_stereo", "stereo"), &SoundFontGenerator::set_stereo);
    ClassDB::bind_method(D_METHOD("get_stereo"), &SoundFontGenerator::get_stereo);
    ClassDB::bind_method(D_METHOD("set_gain", "gain"), &SoundFontGenerator::set_gain);
    ClassDB::bind_method(D_METHOD("get_gain"), &SoundFontGenerator::get_gain);
    ClassDB::bind_method(D_METHOD("set_volume", "volume"), &SoundFontGenerator::set_volume);
    ClassDB::bind_method(D_METHOD("get_volume"), &SoundFontGenerator::get_volume);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "SoundFont"), "set_soundfont", "get_soundfont");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix_rate", PROPERTY_HINT_RANGE, "20,192000,1,suffix:Hz"), "set_mix_rate", "get_mix_rate");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "stereo"), "set_stereo", "get_stereo");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PROPERTY_HINT_RANGE, "-22.0,22.0,0.1,suffix:dB"), "set_gain", "get_gain");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "volume", PROPERTY_HINT_RANGE, "0.0,1.0,0.01,suffix:"), "set_volume", "get_volume");
}

SoundFontGenerator::SoundFontGenerator() {
    // Initialize any variables here.
    time_passed = 0.0f;
    mix_rate = 44100.0f;
    stereo = true;
    gain = 0.0f;
    volume = 1.0f;
    generator = nullptr;
}

void SoundFontGenerator::setup_generator() {
    if (generator) {
        tsf_set_output(generator, stereo ? TSF_STEREO_INTERLEAVED : TSF_MONO, static_cast<int>(mix_rate), gain);
        tsf_set_volume(generator, volume);
    }
}

SoundFontGenerator::~SoundFontGenerator() {
    if (generator) {
        tsf_close(generator);
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

void SoundFontGenerator::set_volume(float p_volume) {
    volume = p_volume;
    setup_generator();
}

float SoundFontGenerator::get_volume() const {
    return volume;
}

void SoundFontGenerator::_process(double delta) {
    time_passed += delta;
}
