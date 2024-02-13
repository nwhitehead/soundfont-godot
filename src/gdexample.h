#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/node.hpp>

namespace godot {

class SoundFont : public Node {
	GDCLASS(SoundFont, Node)

private:
	double time_passed;
    float mix_rate;
    String filename;
    Ref<AudioStream> stream;

protected:
	static void _bind_methods();

public:
	SoundFont();
	~SoundFont();

    void set_stream(Ref<AudioStream> p_stream);
    Ref<AudioStream> get_stream() const;

	void _process(double delta) override;
    void set_mix_rate(float mix_rate);
    float get_mix_rate() const;
};

}

#endif
