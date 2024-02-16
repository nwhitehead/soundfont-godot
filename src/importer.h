#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/editor_import_plugin.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_saver.hpp>

using namespace godot;

class SoundFontImportPlugin : public EditorImportPlugin {
    GDCLASS(SoundFontImportPlugin, EditorImportPlugin)

protected:
    static void _bind_methods() {}

public:
    PackedStringArray _get_recognized_extensions() {
        PackedStringArray result{};
        result.append("sf2");
        return result;
    }

    String _get_importer_name() {
        return "SoundFont2";
    }

    String _get_visible_name() {
        return "SoundFont2";
    }

    String _get_resource_type() {
        return "SoundFont";
    }
    float _get_priority() {
        return 2.0f;
    }

    int _get_import_order() {
        return 2;
    }

    String _get_save_extension() const {
        return "tres";
    }

    TypedArray<Dictionary> _get_import_options(const String &path, int32_t preset_index) {
        TypedArray<Dictionary> result{};
        return result;
    }

    int _get_preset_count() {
        return 0;
    }

	Error _import(const String &source_file,
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

};

class SoundFontPlugin : public EditorPlugin {
    GDCLASS(SoundFontPlugin, EditorPlugin)

protected:
    static void _bind_methods() {}

public:
    void _enter_tree() override {
        Ref<SoundFontImportPlugin> x;
        x.instantiate();
        add_import_plugin(x);
    }
    void _exit_tree() override {
    }
};
