extends Node2D

var sf: SoundFont = null
var sfg: SoundFontGenerator = null
var playback: AudioStreamPlayback = null # Actual playback stream, assigned in _ready().

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
	sfg.note_on(0, 44 + 0, 1.0)
	sfg.note_on(0, 44 + 7, 1.0)
	sfg.note_on(0, 44 + 12, 1.0)
	sfg.note_on(0, 44 + 12 + 4, 1.0)
	var out = sfg.render(10)
	print(out)

func _fill_buffer():
	var samples: int = playback.get_frames_available()
	if samples > 0:
		playback.push_buffer(sfg.render(samples))


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	_fill_buffer()
