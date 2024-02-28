#ifndef SOUNDFONTIMPORT_H
#define SOUNDFONTIMPORT_H

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/editor_import_plugin.hpp>

using namespace godot;

class SoundFontImportPlugin : public EditorImportPlugin {
    GDCLASS(SoundFontImportPlugin, EditorImportPlugin)

protected:
    static void _bind_methods();

public:
    PackedStringArray _get_recognized_extensions();
    String _get_importer_name();
    String _get_visible_name();
    String _get_resource_type();
    float _get_priority();
    int _get_import_order();
    String _get_save_extension() const override;
    TypedArray<Dictionary> _get_import_options(const String &path, int32_t preset_index);
    int _get_preset_count();
	Error _import(const String &source_file,
                  const String &save_path,
                  const Dictionary &options,
                  const TypedArray<String> &platform_variants,
                  const TypedArray<String> &gen_files) const override;
};

class SoundFontPlugin : public EditorPlugin {
    GDCLASS(SoundFontPlugin, EditorPlugin)

protected:
    static void _bind_methods();

public:
    void _enter_tree() override;
    void _exit_tree() override;
};

#endif