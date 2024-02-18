
#ifndef SOUNDFONTEVENT_H
#define SOUNDFONTEVENT_H

enum class EventType {
    NOTE_OFF, NOTE_ON
};

class Event {
    double time;
    EventType event_type;
    int preset_index;
    int key;
    float velocity;
public:
    Event();
    Event(double time, EventType event_type, int preset_index, int key, float velocity);
};

#endif
