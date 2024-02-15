extends Node2D

#var sfg: SoundFontGenerator = null
#var playback: AudioStreamPlayback = null
# 42 # cello
# 58 # tuba
var preset: int = 0
var player: SFPlayer = null

func _ready():
	player = $SFPlayer
	print(player.get_playback_position())
	print(player.get_stream().get_buffer_length())
	print(AudioServer.get_time_to_next_mix())
	print(player.get_presetname(0))
	#player.note_on(0, 44, 1.0)

func _input(event):
	#var playback:AudioStreamGeneratorPlayback = player.get_stream_playback();
	#var pos = player.get_playback_position()
	#print(pos, " ", AudioServer.get_time_to_next_mix(), " ", AudioServer.get_output_latency())
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
			player.note_on(preset, 44 + notes[note], 1.0)
		if event.is_action_released(note):
			player.note_off(preset, 44 + notes[note])
