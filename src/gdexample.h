#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include <godot_cpp/classes/audio_stream_generator.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>

#include "tsf/tsf.h"

namespace godot {

/// A resource the represents the .sf2 file contents
// Uses PackedByteArray in interface with gdscript because I couldn't get Vector<> working for that.
// Explicitly includes `sfdata` in the properties exported so that it will be serialized.
class SoundFont : public Resource {

    GDCLASS(SoundFont, Resource)

private:
    Vector<uint8_t> sfdata;

protected:
    static void _bind_methods();

public:
    void set_data(const PackedByteArray &p_data);
    PackedByteArray get_data() const;
};

class SFPlayer : public AudioStreamPlayer {

    GDCLASS(SFPlayer, AudioStreamPlayer)

private:
    Ref<SoundFont> soundfont;
    tsf * generator;
    float gain;
    int max_voices;
    bool prefilled;
    double time;
    float goal_available_ratio;
    int max_samples_available;

protected:
    static void _bind_methods();
    PackedVector2Array render(int samples);

public:
    SFPlayer();
    ~SFPlayer();

    void set_soundfont(Ref<SoundFont> p_soundfont);
    Ref<SoundFont> get_soundfont() const;
    void setup_generator();

    void set_gain(float gain);
    float get_gain() const;
    void set_max_voices(int max_voices);
    int get_max_voices() const;
    void set_goal_available_ratio(float goal_available_ratio);
    float get_goal_available_ratio() const;

    int get_presetindex(int bank, int preset_number) const;
    int get_presetcount() const;
    String get_presetname(int preset_index) const;
    String bank_get_presetname(int bank, int preset_number) const;
    void note_on(int preset_index, int key, float velocity);
    void bank_note_on(int bank, int preset_number, int key, float velocity);
    void note_off(int preset_index, int key);
    void bank_note_off(int bank, int preset_number, int key);

    void _process(double delta) override;
};



}

#endif
