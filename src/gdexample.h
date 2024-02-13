#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include <godot_cpp/classes/resource_importer.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/node.hpp>

namespace godot {

class SoundFontImporter : public ResourceImporter {
    GDCLASS(SoundFontImporter, ResourceImporter)

protected:
    static void _bind_methods();

public:
    String get_importer_name() const;

};

class SoundFont : public Resource {
    GDCLASS(SoundFont, Resource)

protected:
    static void _bind_methods();
};

class SoundFontPlayer : public Node {
    GDCLASS(SoundFontPlayer, Node)

private:
    double time_passed;
    float mix_rate;
    String filename;
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
    void set_filename(String p_filename);
    String get_filename() const;
};

}

#endif
