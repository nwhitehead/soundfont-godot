
#ifndef MIDI_H
#define MIDI_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/editor_import_plugin.hpp>

#include "tsf/tml.h"

namespace godot {

/// A resource that represents the MIDI file contents, in parsed form
// General idea is structure-of-arrays format, so we can use PackedByteArray
class Midi : public Resource {

    GDCLASS(Midi, Resource)
private:
    Array events;

protected:
    static void _bind_methods();

public:

    enum MessageType
    {
        NOTE_OFF = 0x80,
        NOTE_ON = 0x90,
        KEY_PRESSURE = 0xA0,
        CONTROL_CHANGE = 0xB0,
        PROGRAM_CHANGE = 0xC0,
        CHANNEL_PRESSURE = 0xD0,
        PITCH_BEND = 0xE0,
        SET_TEMPO = 0x51
    };

    void set_events(const Array &time);
    Array get_events() const;
};

class MidiImportPlugin : public EditorImportPlugin {

    GDCLASS(MidiImportPlugin, EditorImportPlugin)

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

class MidiPlugin : public EditorPlugin {

    GDCLASS(MidiPlugin, EditorPlugin)

protected:
    static void _bind_methods();

public:
    void _enter_tree() override;
    void _exit_tree() override;
};


}

VARIANT_ENUM_CAST(Midi::MessageType)

#endif
