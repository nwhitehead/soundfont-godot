#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

namespace godot {

/// A resource the represents the .sf2 file contents
// Uses PackedByteArray in interface with gdscript because I couldn't get Vector<> working for that.
class SoundFont : public Resource {
    GDCLASS(SoundFont, Resource)

private:
    Vector<uint8_t> sfdata;

protected:
    static void _bind_methods();

public:
    void set_data(const PackedByteArray &p_data);
    PackedByteArray get_data() const;
    int get_first() const;
};

/// The node that can play notes and generate audio from a SoundFont
class SoundFontPlayer : public Node {
    GDCLASS(SoundFontPlayer, Node)

private:
    double time_passed;
    float mix_rate;
    bool stereo;
    Ref<SoundFont> soundfont;

protected:
    static void _bind_methods();

public:
    SoundFontPlayer();
    ~SoundFontPlayer();

    void set_soundfont(Ref<SoundFont> p_resource);
    Ref<SoundFont> get_soundfont() const;

    void _process(double delta) override;
    void set_mix_rate(float mix_rate);
    float get_mix_rate() const;
    void set_stereo(bool stereo);
    bool get_stereo() const;
};

}

#endif
