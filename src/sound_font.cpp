
#include "sound_font.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SoundFont::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_data", "data"), &SoundFont::set_data);
    ClassDB::bind_method(D_METHOD("get_data"), &SoundFont::get_data);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data"), "set_data", "get_data");

}

void SoundFont::set_data(const PackedByteArray &p_data) {
    sfdata.resize(p_data.size());
    std::memcpy(sfdata.ptrw(), p_data.ptr(), p_data.size());
}

PackedByteArray SoundFont::get_data() const {
    PackedByteArray result;
    result.resize(sfdata.size());
    std::memcpy(result.ptrw(), sfdata.ptr(), sfdata.size());
    return result;
}
