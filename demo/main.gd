extends Node2D

var preset: int = 0
var player: SoundFontPlayer = null
var base_note: int = 44

func _ready():
	player = $SoundFontPlayer
	print("sf2 preset: ", player.get_presetname(preset))

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
			player.note_on(preset, base_note + notes[note], 1.0)
		if event.is_action_released(note):
			player.note_off(preset, base_note + notes[note])
