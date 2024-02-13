#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/node.hpp>

namespace godot {

class SoundFontPlayer : public Node {
	GDCLASS(SoundFontPlayer, Node)

private:
	double time_passed;
    float mix_rate;
    String filename;
    Ref<Resource> resource;

protected:
	static void _bind_methods();

public:
	SoundFontPlayer();
	~SoundFontPlayer();

    void set_resource(Ref<Resource> p_resource);
    Ref<Resource> get_resource() const;

	void _process(double delta) override;
    void set_mix_rate(float mix_rate);
    float get_mix_rate() const;
};

}

#endif
