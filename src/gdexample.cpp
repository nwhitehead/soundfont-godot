#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SoundFontPlayer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mix_rate", "hz"), &SoundFontPlayer::set_mix_rate);
	ClassDB::bind_method(D_METHOD("get_mix_rate"), &SoundFontPlayer::get_mix_rate);
    ClassDB::bind_method(D_METHOD("set_resource", "resource"), &SoundFontPlayer::set_resource);
    ClassDB::bind_method(D_METHOD("get_resource"), &SoundFontPlayer::get_resource);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix_rate", PROPERTY_HINT_RANGE, "20,192000,1,suffix:Hz"), "set_mix_rate", "get_mix_rate");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "resource", PROPERTY_HINT_RESOURCE_TYPE, "Resource"), "set_resource", "get_resource");
}

SoundFontPlayer::SoundFontPlayer() {
	// Initialize any variables here.
	time_passed = 0.0;
    mix_rate = 44100.0;
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

void SoundFontPlayer::set_resource(Ref<Resource> p_resource) {
    resource = p_resource;
}

Ref<Resource> SoundFontPlayer::get_resource() const {
    return resource;
}

void SoundFontPlayer::_process(double delta) {
	time_passed += delta;
}
