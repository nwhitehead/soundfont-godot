#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

void SoundFont::_bind_methods() {
}

void SoundFontImporter::_bind_methods() {
}

String SoundFontImporter::get_importer_name() const {
	return "sf2";
}

String SoundFontPlayer::get_filename() const {
    return filename;
}

void SoundFontPlayer::set_filename(String p_filename) {
    filename = p_filename;
}

void SoundFontPlayer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_mix_rate", "hz"), &SoundFontPlayer::set_mix_rate);
    ClassDB::bind_method(D_METHOD("get_mix_rate"), &SoundFontPlayer::get_mix_rate);
    ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont"), &SoundFontPlayer::set_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont"), &SoundFontPlayer::get_soundfont);
    ClassDB::bind_method(D_METHOD("set_filename", "filename"), &SoundFontPlayer::set_filename);
    ClassDB::bind_method(D_METHOD("get_filename"), &SoundFontPlayer::get_filename);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix_rate", PROPERTY_HINT_RANGE, "20,192000,1,suffix:Hz"), "set_mix_rate", "get_mix_rate");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "SoundFont"), "set_soundfont", "get_soundfont");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "filename", PROPERTY_HINT_RESOURCE_TYPE, "Filename"), "set_filename", "get_filename");
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

void SoundFontPlayer::set_soundfont(Ref<SoundFont> p_soundfont) {
    soundfont = p_soundfont;
}

Ref<SoundFont> SoundFontPlayer::get_soundfont() const {
    return soundfont;
}

void SoundFontPlayer::_process(double delta) {
    time_passed += delta;

	Ref<FileAccess> file = FileAccess::open(filename, FileAccess::READ);
}
