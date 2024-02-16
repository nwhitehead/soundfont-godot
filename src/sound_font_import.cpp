
#include "sound_font_import.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_saver.hpp>

#include "sound_font.h"

using namespace godot;

void SoundFontImportPlugin::_bind_methods() {
    // No gdscript bindings needed
}

PackedStringArray SoundFontImportPlugin::_get_recognized_extensions() {
    PackedStringArray result{};
    result.append("sf2");
    return result;
}

String SoundFontImportPlugin::_get_importer_name() {
    return "SoundFont2";
}

String SoundFontImportPlugin::_get_visible_name() {
    return "SoundFont2";
}

String SoundFontImportPlugin::_get_resource_type() {
    return "SoundFont";
}

float SoundFontImportPlugin::_get_priority() {
    return 2.0f;
}

int SoundFontImportPlugin::_get_import_order() {
    return 0;
}

String SoundFontImportPlugin::_get_save_extension() const {
    return "tres";
}

TypedArray<Dictionary> SoundFontImportPlugin::_get_import_options(const String &path, int32_t preset_index) {
    TypedArray<Dictionary> result{};
    return result;
}

int SoundFontImportPlugin::_get_preset_count() {
    return 0;
}

Error SoundFontImportPlugin::_import(const String &source_file,
                                     const String &save_path,
                                     const Dictionary &options,
                                     const TypedArray<String> &platform_variants,
                                     const TypedArray<String> &gen_files) const {
    PackedByteArray data = FileAccess::get_file_as_bytes(source_file);
    Ref<SoundFont> sf;
    sf.instantiate();
    sf->set_data(data);
    return ResourceSaver::get_singleton()->save(sf, String(save_path) + String(".") + _get_save_extension());
}

void SoundFontPlugin::_bind_methods() {
    // No gdscript bindings needed
}

void SoundFontPlugin::_enter_tree() {
    Ref<SoundFontImportPlugin> x;
    x.instantiate();
    add_import_plugin(x);
}

void SoundFontPlugin::_exit_tree() {
}
