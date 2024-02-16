
#include "soundfont.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

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
        return "SoundFont2 importer";
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

    String _get_save_extension() {
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
        UtilityFunctions::print("SoundFontImportPlugin _import", " source=", source_file);
        // Ref<FileAccess> file = FileAccess::open(source_file, FileAccess::READ);
        // if (!file.is_valid()) {
        //     return FileAccess::get_open_error();
        // }
        PackedByteArray data = FileAccess::get_file_as_bytes(source_file);
        UtilityFunctions::print("SoundFontImportPlugin _import", " data.size()=", data.size());
        Ref<SoundFont> sf;
        sf.instantiate();
        sf->set_data(data);
        String outpath = String(save_path) + String(".tres");
        UtilityFunctions::print("SoundFontImportPlugin _import", " saving to ", outpath);
        return ResourceSaver::get_singleton()->save(sf, outpath);
        //return Error(0);
    }

};

class SoundFontPlugin : public EditorPlugin {
    GDCLASS(SoundFontPlugin, EditorPlugin)

protected:
    static void _bind_methods() {}

public:
    void _enter_tree() override {
        UtilityFunctions::print("SoundFontPlugin _enter_tree()");
        Ref<SoundFontImportPlugin> x;
        x.instantiate();
        add_import_plugin(x);
        UtilityFunctions::print("SoundFontImportPlugin registered");
    }
};

void initialize_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
        return;
    }

    ClassDB::register_class<SoundFont>();
    ClassDB::register_class<SoundFontPlayer>();
    ClassDB::register_class<SoundFontImportPlugin>();
    ClassDB::register_class<SoundFontPlugin>();
    EditorPlugins::add_by_type<SoundFontPlugin>();
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                        const GDExtensionClassLibraryPtr p_library,
                                        GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

    return init_obj.init();
}
} // extern "C"
