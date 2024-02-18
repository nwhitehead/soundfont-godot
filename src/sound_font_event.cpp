
#include "sound_font_event.h"

Event::Event(double p_time, EventType p_event_type) {
    time = p_time;
    event_type = p_event_type;
    channel = -1;
    bank = -1;
    preset_index = -1;
    key = -1;
    velocity = 0.0f;
}

Event::Event() {
    time = 0.0;
    event_type = EventType::NOTE_OFF;
    channel = -1;
    bank = -1;
    preset_index = -1;
    key = -1;
    velocity = 0.0f;
}
