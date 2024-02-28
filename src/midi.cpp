
#include "midi.h"

#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#define TML_IMPLEMENTATION
#include "tsf/tml.h"

namespace godot {

void Midi::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_events", "events"), &Midi::set_events);
    ClassDB::bind_method(D_METHOD("get_events"), &Midi::get_events);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "events"), "set_events", "get_events");
    BIND_ENUM_CONSTANT(NOTE_OFF);
    BIND_ENUM_CONSTANT(NOTE_ON);
    BIND_ENUM_CONSTANT(KEY_PRESSURE);
    BIND_ENUM_CONSTANT(CONTROL_CHANGE);
    BIND_ENUM_CONSTANT(PROGRAM_CHANGE);
    BIND_ENUM_CONSTANT(CHANNEL_PRESSURE);
    BIND_ENUM_CONSTANT(PITCH_BEND);
    BIND_ENUM_CONSTANT(SET_TEMPO);
}

void Midi::set_events(const Array &p_events) { events = p_events; }

Array Midi::get_events() const { return events; }


void MidiImportPlugin::_bind_methods() {
    // No gdscript bindings needed
}

PackedStringArray MidiImportPlugin::_get_recognized_extensions() {
    PackedStringArray result{};
    result.append("mid");
    result.append("midi");
    return result;
}

String MidiImportPlugin::_get_importer_name() {
    return "Simple MIDI importer";
}

String MidiImportPlugin::_get_visible_name() {
    return "Simple MIDI importer";
}

String MidiImportPlugin::_get_resource_type() {
    return "Midi";
}

float MidiImportPlugin::_get_priority() {
    return 2.0f;
}

int MidiImportPlugin::_get_import_order() {
    return 0;
}

String MidiImportPlugin::_get_save_extension() const {
    return "res";
}

TypedArray<Dictionary> MidiImportPlugin::_get_import_options(const String &/*path*/, int32_t /*preset_index*/) {
    TypedArray<Dictionary> result{};
    return result;
}

int MidiImportPlugin::_get_preset_count() {
    return 0;
}

Error MidiImportPlugin::_import(const String &source_file,
                                const String &save_path,
                                const Dictionary &/*options*/,
                                const TypedArray<String> &/*platform_variants*/,
                                const TypedArray<String> &/*gen_files*/) const {
    // Read in contents of MIDI file
    PackedByteArray data = FileAccess::get_file_as_bytes(source_file);
    // Create empty Midi object
    Ref<Midi> sf;
    sf.instantiate();
    // Parse contents using TML
    tml_message *parsed = tml_load_memory(data.ptr(), data.size());
    if (!parsed) {
        UtilityFunctions::printerr("Could not parse MIDI file");
        return Error::ERR_FILE_CORRUPT;
    }
    Array events{};
    int size = tml_get_info(parsed, nullptr, nullptr, nullptr, nullptr, nullptr);
    tml_message *pos = parsed;
    double current_bpm = 10.0;
    for (int i = 0; i < size; i++) {
        double t = (pos->time) / 1000.0f;
        Dictionary event{};
        event[Variant("t")] = t;
        event[Variant("type")] = pos->type;
        event[Variant("channel")] = pos->channel;
        event[Variant("key")] = pos->key;
        event[Variant("velocity")] = pos->velocity;
        // Update bpm on tempo change
        if (pos->type == TML_SET_TEMPO) {
            double microseconds_per_beat = tml_get_tempo_value(pos);
            current_bpm = 60e6 / microseconds_per_beat;
        }
        event[Variant("bpm")] = current_bpm;
        pos = pos->next;
        events.push_back(event);
    }
    sf->set_events(events);
    tml_free(parsed);
    return ResourceSaver::get_singleton()->save(sf, String(save_path) + String(".") + _get_save_extension());
}

void MidiPlugin::_bind_methods() {
    // No gdscript bindings needed
}

void MidiPlugin::_enter_tree() {
    Ref<MidiImportPlugin> x;
    x.instantiate();
    add_import_plugin(x);
}

void MidiPlugin::_exit_tree() {
}

}

