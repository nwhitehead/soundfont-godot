#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class SoundFont : public Node {
	GDCLASS(SoundFont, Node)

private:
	double time_passed;
    float mix_rate;
    String filename;

protected:
	static void _bind_methods();

public:
	SoundFont();
	~SoundFont();

	void _process(double delta) override;
    void set_mix_rate(float mix_rate);
    float get_mix_rate() const;
};

}

#endif
