#ifndef SOUNDFONTPLAYER_H
#define SOUNDFONTPLAYER_H

#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>

#include "sound_font.h"
#include "tsf/tsf.h"

namespace godot {

class SoundFontPlayer : public AudioStreamPlayer {

    GDCLASS(SoundFontPlayer, AudioStreamPlayer)

private:
    Ref<SoundFont> soundfont;
    tsf * generator;
    float gain;
    int max_voices;
    double time;
    float goal_available_ratio;
    int max_samples_available;

protected:
    static void _bind_methods();
    PackedVector2Array render(int samples);

public:
    SoundFontPlayer();
    ~SoundFontPlayer();

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
    void note_on(int preset_index, int key, float velocity);
    void note_off(int preset_index, int key);

    double get_time() const;

    void _physics_process();
};



}

#endif
