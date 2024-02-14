extends Node2D

#var sfg: SoundFontGenerator = null
#var playback: AudioStreamPlayback = null
# 42 # cello
# 58 # tuba
var preset: int = 0
var player: SFPlayer = null

func _ready():
	player = $SFPlayer
	print(player.get_presetname(0))
	#sfg = $SoundFontGenerator
	#playback = sfg.get_node("AudioStreamPlayer").get_stream_playback();
	#print("From SoundFontGenerator: ", sfg.get_presetname(preset))
	## Avoid startup audio glitch by prefilling buffer
	#_fill_buffer(1/60)

#func _fill_buffer(delta):
	#var samples: int = playback.get_frames_available()
	##var our_samples: int = min(samples, 44100/100) + 10
	#var our_samples: int = min(samples, delta * 44100)
	## print(delta, " : ", our_samples, " / ", samples)
	#if our_samples > 0:
		#playback.push_buffer(sfg.render(our_samples))
#
#func _input(event):
	#var notes = {
		#"note_c": 0,
		#"note_d": 2,
		#"note_e": 4,
		#"note_f": 5,
		#"note_g": 7,
		#"note_a": 9,
		#"note_b": 11,
		#"note_c2": 12,
		#"note_d2": 14,
	#}
	#for note in notes:
		#if event.is_action_pressed(note):
			#sfg.note_on(preset, 44 + notes[note], 1.0)
		#if event.is_action_released(note):
			#sfg.note_off(preset, 44 + notes[note])
#
#func _process(delta):
	#_fill_buffer(delta)
