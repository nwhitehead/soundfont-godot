# SoundFont Player for Godot

This project is a `GDExtension` that allows the Godot game engine to play
SoundFont instruments (`.sf2` files). This lets your Godot game control
instruments and notes programmatically during gameplay.

Some reasons you might want to do this:

-   You need precise control of rhythm and timing of musical events in game
-   You want a retro MIDI music aesthetic
-   To change rhythm, pitches, harmony of game music based on gameplay events
-   You want to do general procedural music generation
-   To create music composing tools

Features of this extension:

-   Supports SoundFonts in files using extensions `sf2`, `sf3`, and `sfo`.
-   Playback multiple channels, voices, and instruments at once.
-   Sample-accurate playback API to schedule events at precise times.
-   Low latency and overhead, audio sample generation done in C++ (actual latency depends on
    audio drivers).
-   Load and playback MIDI files with full control over notes.

## File Formats

SoundFont instruments are normally stored with the `.sf2` file extension. The
`.sf3` and `.sfo` extensions are compressed file formats that use Ogg/Vorbis to
encode audio data. Each file is one "SoundFont" and may contain data for
multiple instruments. Many SoundFonts are compatible with General MIDI format
which means they organize programs in a standard way that includes 128
instruments and 47 percussion sounds. Other SoundFonts are designed to mimic
a specific synthesizer or single physical instrument.

## Installation

To install the extension in your Godot project, unzip the release zip file and
put the `soundfont-gdextension` directory inside an `addons` directory in your
project root. If the `addons` directory does not exist, create it first. Restart
the Godot editor and open your project.

The release distribution file includes support for the following platforms:

-   Linux (x86_64)
-   Windows (x86_64)
-   MacOS

Other platforms require building from source.

## Organization

Once the extension is installed you should be able to put `sf2` files in your
project and Godot should recognize them as `SoundFont` class.

You should be able to create nodes of class `SoundFontPlayer`. The
`SoundFontPlayer` class is an extension of `AudioStreamPlayer` that can generate
audio samples from `SoundFont` resources.

The `SoundFontPlayer` has properties:

-   `soundfont` which is a new resource type representing the SoundFont to use.
-   `gain` which determines total volume, in dB.
-   `max_voices` which determines how many simultaneous voices can be played
    (multiple voices may be used for 1 instrument note).
-   `goal_available_ratio` which is a ratio of how much buffer the extension will try
    to keep unused.

To get a note to play, you need create a `SoundFontPlayer` node and setup the
`soundfont` field to be a valid `sf2` file. Other settings should automatically
be set to reasonable defaults. Next a script in the project should ask the
`SoundFontPlayer` to play a note. A script `main.gd` connected to the root node
could do:

    $SoundFontPlayer.note_on(0, 0, 79, 1.0)

The first argument is `time`, second is `preset_index`, then `key`, and finally
`velocity`. So the command immediately schedules a `NOTE_ON` event for preset
`0`, MIDI key `79` at full velocity. MIDI key `79` is `G4`.

## Timing

Timing is measured in seconds. Note that audio time is usually close to other
time sources in Godot **but may drift over time**. To get the current time of
audio that has already been pushed to the audio driver call
`SoundFontPlayer.get_time()`. Any method calls to `SoundFontPlayer` with a
`time` of this time or lower will happen as soon as possible at the beginning of
the next audio callback. Method calls that give `time` higher than this value
will be scheduled for that audio position. These events can be within the time
of the next audio callback and should be handled correctly to within one audio
sample.

## Latency

The audio generation callback for this extension runs in the
`_physics_process()` callback at the physics processing framerate. For low
latency audio response it is recommended that you keep the physics framerate at
60 FPS.

Audio generation is also buffered by the buffer in `SoundFontPlayer` in the
`AudioStreamPlayer` stream. The `stream` is a `Generator` that has a
`buffer_length`. This should be set high enough to prevent audio underruns and
glitches. To avoid high latency, avoid setting this buffer too big. The
default value for the buffer length is `0.1s`.

The audio buffer is filled to some ratio by `SoundFontPlayer`, set by
`goal_available_ratio`. A zero value for `goal_available_ratio` will aim to fill
the audio buffer completely. This means if the buffer is `0.1s` and it is filled
completely, there will be at least `0.1s` of latency due to the buffering. A
high value of `goal_available_ratio` will attempt to leave most of the buffer
unused. So if `goal_available_ratio` is `0.8` and the buffer is `0.1s` then the
audio generator will try to use only about `0.02s` of the buffer.

To get low latency that will avoid audio glitching most of the time it is
recommended to set `goal_available_ratio` to a ratio that leaves at least
`0.02s` of buffer available. For the default `0.1s` buffer this means leaving
`goal_available_ratio` at `0.8` or lower.

## Scheduling Events

The simplest way to deal with timing is to just decide what methods to call
using any mechanism you want, then always pass `0` to the `time` argument for
all calls. This means that events will happen as soon as possible with lowest
latency. The only downside is that for periodic sounds such as drum beats there
may be some jitter in the timing. Each drum event will have low latency but
because the latency of each event is random it may be audible as "not feeling
locked-in".

For situations that benefit from less jitter and are more predictable (e.g. drum
beats and music playback), a good general strategy is to use
`_physics_process()` to schedule events some amount of time into the future. In
`_physics_process()`, call `SoundFontPlayer.get_time()` to get the current audio
time. Then schedule events that have not yet been scheduled that should happen
between this time and a short time ahead `delta` in the future. This will look
like calling methods `SoundFontPlayer.add_note()` with `time` arguments that are
larger than `SoundFontPlayer.get_time()`.

### Choosing `delta`

When scheduling events in `_physics_process()` you need to consider events at
least up through:

    delta = 1.0 / Engine.get_physics_ticks_per_second()

But this `delta` value is not big enough. Because the audio callback duration
may not match the physics framerate exactly you should schedule ahead more.
You can use a multiple of the above value:

    delta = 3.0 / Engine.get_physics_ticks_per_second()

This will schedule events 3 callbacks ahead.

## Building from Source

The source files are self-contained and do not link to any libraries. CMake is used for building.

To build from source locally, clone the repository and use CMake:

    git clone https://github.com/nwhitehead/soundfont-godot.git
    cd soundfont-godot
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make -j8

This should result in a directory `soundfont-gdextension` inside of `build` that
is suitable for copying into an `addon` directory of a Godot project for the
current platform. You normally just need `Debug` version for using the Godot
editor. You will also need the `Release` build for exporting projects for the
new platform.
