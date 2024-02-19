
#include "midi.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_saver.hpp>

#define TML_IMPLEMENTATION
#include "tsf/tml.h"

namespace godot {

void Midi::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_time", "time"), &Midi::set_time);
    ClassDB::bind_method(D_METHOD("get_time"), &Midi::get_time);
    ClassDB::bind_method(D_METHOD("set_type", "type"), &Midi::set_type);
    ClassDB::bind_method(D_METHOD("get_type"), &Midi::get_type);
    ClassDB::bind_method(D_METHOD("set_channel", "channel"), &Midi::set_channel);
    ClassDB::bind_method(D_METHOD("get_channel"), &Midi::get_channel);
    ClassDB::bind_method(D_METHOD("set_arg0", "arg0"), &Midi::set_arg0);
    ClassDB::bind_method(D_METHOD("get_arg0"), &Midi::get_arg0);
    ClassDB::bind_method(D_METHOD("set_arg1", "arg1"), &Midi::set_arg1);
    ClassDB::bind_method(D_METHOD("get_arg1"), &Midi::get_arg1);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "time"), "set_time", "get_time");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "type"), "set_type", "get_type");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "channel"), "set_channel", "get_channel");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "arg0"), "set_arg0", "get_arg0");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "arg1"), "set_arg1", "get_arg1");
}

void Midi::set_time(const PackedFloat32Array &p_time) { time = p_time; }

PackedFloat32Array Midi::get_time() const { return time; }

void Midi::set_type(const PackedByteArray &p_type) { type = p_type; }

PackedByteArray Midi::get_type() const { return type; }

void Midi::set_channel(const PackedByteArray &p_channel) { channel = p_channel; }

PackedByteArray Midi::get_channel() const { return channel; }

void Midi::set_arg0(const PackedByteArray &p_arg0) { arg0 = p_arg0; }

PackedByteArray Midi::get_arg0() const { return arg0; }

void Midi::set_arg1(const PackedByteArray &p_arg1) { arg1 = p_arg1; }

PackedByteArray Midi::get_arg1() const { return arg1; }

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

TypedArray<Dictionary> MidiImportPlugin::_get_import_options(const String &path, int32_t preset_index) {
    TypedArray<Dictionary> result{};
    return result;
}

int MidiImportPlugin::_get_preset_count() {
    return 0;
}

Error MidiImportPlugin::_import(const String &source_file,
                                const String &save_path,
                                const Dictionary &options,
                                const TypedArray<String> &platform_variants,
                                const TypedArray<String> &gen_files) const {
    // Read in contents of MIDI file
    PackedByteArray data = FileAccess::get_file_as_bytes(source_file);
    // Create empty Midi object
    Ref<Midi> sf;
    sf.instantiate();
    UtilityFunctions::print("Importing MIDI file");
    // Parse contents using TML
    tml_message *parsed = tml_load_memory(data.ptr(), data.size());
    int size = tml_get_info(parsed, nullptr, nullptr, nullptr, nullptr, nullptr);

    PackedFloat32Array time;
    PackedByteArray type;
    PackedByteArray channel;
    PackedByteArray arg0;
    PackedByteArray arg1;
    time.resize(size);
    type.resize(size);
    channel.resize(size);
    arg0.resize(size);
    arg1.resize(size);
    tml_message *pos = parsed;
    for (int i = 0; i < size; i++) {
        time[i] = (pos->time) / 1000.0f;
        type[i] = pos->type;
        channel[i] = pos->channel;
        pos = pos->next;
    }
    sf->set_time(time);
    sf->set_type(type);
    sf->set_channel(channel);
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

