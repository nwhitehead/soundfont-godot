extends Node2D

var preset: int = 0
var player: SoundFontPlayer = null
var base_note: int = 44
var arpnotes = [0, 4, 7, 12]
var arpseq = [0, 1, 2, 3, 2, 1, 0, 1]
var arppos = -1
var oldtime = 0

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

func _on_arp_timer_timeout():
	player.note_off_all()
	arppos += 1
	if arppos >= len(arpseq):
		arppos = 0
	var time = player.get_time()
	var delta = time - oldtime
	print('note_on delta=', delta)
	oldtime = time
	player.note_on(preset, base_note + arpnotes[arpseq[arppos]], 1.0)

func _on_button_button_down_c():
	# C E G C
	arpnotes = [0, 4, 7, 12]

func _on_button_button_down_g():
	# B D G D
	arpnotes = [-1, 2, 7, 11]
