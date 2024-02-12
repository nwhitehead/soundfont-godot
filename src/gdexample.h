#ifndef GDSOUNDFONT_H
#define GDSOUNDFONT_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class GDSoundFont : public Node {
	GDCLASS(GDSoundFont, Node)

private:
	double time_passed;
    float mix_rate;

protected:
	static void _bind_methods();

public:
	GDSoundFont();
	~GDSoundFont();

	void _process(double delta) override;
    void set_mix_rate(float mix_rate);
    float get_mix_rate() const;
};

}

#endif
