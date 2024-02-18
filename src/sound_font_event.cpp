
#include "sound_font_event.h"

Event::Event() {
    time = 0.0;
    event_type = EventType::NOTE_OFF;
    preset_index = 0;
    key = 0;
    velocity = 0.0f;
}

Event::Event(double p_time, EventType p_event_type, int p_preset_index, int p_key, float p_velocity) {
    time = p_time;
    event_type = p_event_type;
    preset_index = p_preset_index;
    key = p_key;
    velocity = p_velocity;
}
