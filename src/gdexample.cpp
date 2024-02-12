#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void GDSoundFont::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mix_rate", "hz"), &GDSoundFont::set_mix_rate);
	ClassDB::bind_method(D_METHOD("get_mix_rate"), &GDSoundFont::get_mix_rate);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix_rate", PROPERTY_HINT_RANGE, "20,192000,1,suffix:Hz"), "set_mix_rate", "get_mix_rate");
}

GDSoundFont::GDSoundFont() {
	// Initialize any variables here.
	time_passed = 0.0;
    mix_rate = 44100.0;
}

GDSoundFont::~GDSoundFont() {
	// Add your cleanup here.
}

void GDSoundFont::set_mix_rate(float p_mix_rate) {
    mix_rate = p_mix_rate;
}

float GDSoundFont::get_mix_rate() const {
    return mix_rate;    
}

void GDSoundFont::_process(double delta) {
	time_passed += delta;

	//Vector2 new_position = Vector2(10.0 + (10.0 * sin(time_passed * 2.0)), 10.0 + (10.0 * cos(time_passed * 1.5)));

	//set_position(new_position);
}
