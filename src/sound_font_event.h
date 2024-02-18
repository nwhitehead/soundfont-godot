
#ifndef SOUNDFONTEVENT_H
#define SOUNDFONTEVENT_H

enum class EventType {
    NOTE_OFF, NOTE_ON, NOTE_OFF_ALL,
    SET_PRESETINDEX, SET_PRESETNUMBER, SET_BANK,
    SET_PAN, SET_VOLUME, SET_PITCHWHEEL, SET_PITCHRANGE, SET_TUNING,
};

class Event {
public:
    double time;
    EventType event_type;
    int channel;
    int bank;
    int preset_index;
    int key;
    float velocity;

    Event();
    Event(double time, EventType event_type);
};

#endif
