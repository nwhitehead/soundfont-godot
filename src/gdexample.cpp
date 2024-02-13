#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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

void SoundFontPlayer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_mix_rate", "hz"), &SoundFontPlayer::set_mix_rate);
    ClassDB::bind_method(D_METHOD("get_mix_rate"), &SoundFontPlayer::get_mix_rate);
    ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont"), &SoundFontPlayer::set_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont"), &SoundFontPlayer::get_soundfont);
    ClassDB::bind_method(D_METHOD("set_stereo", "stereo"), &SoundFontPlayer::set_stereo);
    ClassDB::bind_method(D_METHOD("get_stereo"), &SoundFontPlayer::get_stereo);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "SoundFont"), "set_soundfont", "get_soundfont");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix_rate", PROPERTY_HINT_RANGE, "20,192000,1,suffix:Hz"), "set_mix_rate", "get_mix_rate");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "stereo"), "set_stereo", "get_stereo");
}

SoundFontPlayer::SoundFontPlayer() {
    // Initialize any variables here.
    time_passed = 0.0;
    mix_rate = 44100.0;
    stereo = true;
}

SoundFontPlayer::~SoundFontPlayer() {
    // Add your cleanup here.
}

void SoundFontPlayer::set_mix_rate(float p_mix_rate) {
    mix_rate = p_mix_rate;
}

float SoundFontPlayer::get_mix_rate() const {
    return mix_rate;    
}

void SoundFontPlayer::set_soundfont(Ref<SoundFont> p_soundfont) {
    soundfont = p_soundfont;
    if (soundfont.is_valid()) {
        UtilityFunctions::print("SoundFontPlayer set_soundfont called, size=", soundfont->get_data().size());
    } else {
        UtilityFunctions::print("SoundFontPlayer set_soundfont called (empty soundfont)");
    }
}

Ref<SoundFont> SoundFontPlayer::get_soundfont() const {
    return soundfont;
}

void SoundFontPlayer::set_stereo(bool p_stereo) {
    stereo = p_stereo;
}

bool SoundFontPlayer::get_stereo() const {
    return stereo;
}

void SoundFontPlayer::_process(double delta) {
    time_passed += delta;
    
}
