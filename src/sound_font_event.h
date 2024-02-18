
#ifndef SOUNDFONTEVENT_H
#define SOUNDFONTEVENT_H

enum class EventType {
    NOTE_OFF, NOTE_ON, NOTE_OFF_ALL
};

class Event {
public:
    double time;
    EventType event_type;
    int preset_index;
    int key;
    float velocity;

    Event();
    Event(double time, EventType event_type, int preset_index, int key, float velocity);
};

#endif
