
#ifndef SOUNDFONTPLAYER_H
#define SOUNDFONTPLAYER_H

#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>

#include "sound_font.h"
#include "sound_font_event.h"
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
    int process_count;
    Vector<Event> events;
    int configured_mix_rate;
    int configured_max_voices;
    float configured_gain;

protected:
    static void _bind_methods();
    PackedVector2Array render(int samples);
    void setup_generator();
    void add_event(const Event &event);
    void do_event(const Event &event);

public:
    SoundFontPlayer();
    ~SoundFontPlayer();

    void set_soundfont(Ref<SoundFont> p_soundfont);
    Ref<SoundFont> get_soundfont();

    void set_gain(float gain);
    float get_gain();
    void set_max_voices(int max_voices);
    int get_max_voices();
    void set_goal_available_ratio(float goal_available_ratio);
    float get_goal_available_ratio();

    double get_time();
    int get_presetindex(int bank, int preset_number);
    int get_presetcount();
    String get_presetname(int preset_index);
    int get_active_voice_count();

    void note_on(double time, int preset_index, int key, float velocity);
    void note_off(double time, int preset_index, int key);
    void note_off_all(double time);
    void clear_events();

    void channel_set_presetindex(double time, int channel, int preset_index);
    void channel_set_presetnumber(double time, int channel, int preset_number, bool drums);
    void channel_set_bank(double time, int channel, int bank);
    void channel_set_pan(double time, int channel, float pan);
    void channel_set_volume(double time, int channel, float volume);
    void channel_set_pitchwheel(double time, int channel, int pitch_wheel);
    void channel_set_pitchrange(double time, int channel, float pitch_range);
    void channel_set_tuning(double time, int channel, float tuning);
    void channel_note_on(double time, int channel, int key, float velocity);
    void channel_note_off(double time, int channel, int key);

    void _physics_process();
};



}

#endif
