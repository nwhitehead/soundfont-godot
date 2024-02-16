
#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "sound_font.h"
#include "sound_font_player.h"
#include "sound_font_import.h"

using namespace godot;

void initialize_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        ClassDB::register_class<SoundFont>();
        ClassDB::register_class<SoundFontPlayer>();
    }
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        ClassDB::register_class<SoundFontImportPlugin>();
        ClassDB::register_class<SoundFontPlugin>();
        EditorPlugins::add_by_type<SoundFontPlugin>();
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                        const GDExtensionClassLibraryPtr p_library,
                                        GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
} // extern "C"
