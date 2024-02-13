extends Node2D

var sf: SoundFont = null
var sfg: SoundFontGenerator = null
var playback: AudioStreamPlayback = null # Actual playback stream, assigned in _ready().
var time: float = 0.0

# Called when the node enters the scene tree for the first time.
func _ready():
	sf = SoundFont.new()
	# var file = FileAccess.open('res://example_broken.sf2', FileAccess.READ)
	var file = FileAccess.open('res://example.sf2', FileAccess.READ)
	var data = file.get_buffer(file.get_length())
	sf.set_data(data)
	sfg = $SoundFontGenerator
	playback = sfg.get_node("AudioStreamPlayer").get_stream_playback();
	sfg.set_soundfont(sf)
	print(sfg.get_presetname(0))
	sfg.note_on(0, 44 - 24, 1.0)
	var out = sfg.render(10)
	print(out)

func _fill_buffer():
	var samples: int = playback.get_frames_available()
	if samples > 0:
		playback.push_buffer(sfg.render(samples))

func _input(event):
	var notes = {
		"note_c": 0,
		"note_d": 2,
		"note_e": 4,
		"note_f": 5,
		"note_g": 7,
		"note_a": 9,
		"note_b": 11,
		"note_c2": 12,
		"note_d2": 14,
	}
	for note in notes:
		if event.is_action_pressed(note):
			sfg.note_on(0, 44 + notes[note], 1.0)
		if event.is_action_released(note):
			sfg.note_off(0, 44 + notes[note])

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	time += delta
	_fill_buffer()
