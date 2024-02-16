#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

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

}

#endif
