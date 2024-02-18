extends Node2D

var preset: int = 0
var player: SoundFontPlayer = null
var base_note: int = 44
var arpnotes = [0, 4, 7, 12]
var arpseq = [0, 1, 2, 3, 2, 1, 0, 1]
var arppos = -1
var oldtime = 0

var time = 0

func _ready():
	player = $SoundFontPlayer
	print("sf2 preset: ", player.get_presetname(preset))

func _process(delta):
	var ptime = player.get_time()
	var dur = 0.121
	while time < ptime + 1.0 / Engine.get_physics_ticks_per_second() + 0.1:
		player.note_on(time, 0, base_note + arpnotes[arpseq[arppos]], 1.0)
		player.note_off(time + dur/2, 0, base_note + arpnotes[arpseq[arppos]])
		time += dur
		arppos += 1
		if arppos >= len(arpseq):
			arppos = 0

func _on_button_button_down_c():
	# C E G C
	arpnotes = [0, 4, 7, 12]

func _on_button_button_down_g():
	# B D G D
	arpnotes = [-1, 2, 7, 11]
