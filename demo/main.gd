extends Node2D

var play_arp: bool = false
var play_drums: bool = false
var play_song: bool = true
var preset: int = 27 # 25
var drum_kit: int = 20
var preset_drum: int = 35
var preset_snare: int = 40
var preset_hihat: int = 42
var player: SoundFontPlayer = null
var base_note: int = 44
var arpnotes = [0, 4, 7, 12]
var arpseq = [0, 1, 2, 3, 2, 1, 0, 1]
var arppos = -1
var oldtime = 0

var time = 0
var song
var song_time = 0
var song_position = 0

func _ready():
	song = ResourceLoader.load('res://1080-c01.mid')
	player = $SoundFontPlayer
	print("sf2 preset: ", player.get_presetname(preset))
	player.channel_set_presetnumber(0, 1, drum_kit, true)
	player.channel_set_presetnumber(0, 2, preset, false)
	player.channel_set_presetnumber(0, 3, 41, false)
	player.channel_set_presetnumber(0, 4, 42, false)
	player.channel_set_presetnumber(0, 5, 43, false)
	player.channel_set_presetnumber(0, 6, 44, false)

func _process(delta):
	var ptime = player.get_time()
	var dur = 0.171
	var end_time = ptime + 1.0 / Engine.get_physics_ticks_per_second() + 0.1
	while time < end_time:
		if play_arp:
			var note = base_note + arpnotes[arpseq[arppos]]
			player.channel_note_on(time, 2, note, 0.5)
			player.channel_note_off(time + dur, 2, note)
		if play_drums:
			if arppos == 0:
				player.channel_note_on(time, 1, preset_drum, 1.0)
				player.channel_note_off(time + dur / 2, 1, preset_drum)
				print("num_voices=", player.get_active_voice_count())
			else:
				if arppos == 4:
					player.channel_note_on(time, 1, preset_snare, 1.0)
					player.channel_note_off(time + dur / 2, 1, preset_snare)
				else:
					player.channel_note_on(time, 1, preset_hihat, 1.0)
					player.channel_note_off(time + dur / 2, 1, preset_hihat)
		time += dur
		arppos += 1
		if arppos >= len(arpseq):
			arppos = 0			

	while song_position < song.get_time().size() and song.get_time()[song_position] < end_time:
		var t = song.get_time()[song_position]
		var type = song.get_type()[song_position]
		var channel = song.get_channel()[song_position]
		var key = song.get_arg0()[song_position]
		var vel = song.get_arg1()[song_position] / 255.0
		song_position += 1
		if type == Midi.NOTE_ON:
			if play_song:
				player.channel_note_on(t, 3 + channel, key, vel)


func _on_button_button_down_c():
	# C E G C
	player.channel_set_pan(0, 2, 0)
	arpnotes = [0, 4, 7, 12]

func _on_button_button_down_g():
	# B D G D
	player.channel_set_pan(0, 2, 1.0)
	arpnotes = [-1, 2, 7, 11]

func _on_arp_toggle_toggled(toggled_on):
	play_arp = toggled_on

func _on_drum_toggle_toggled(toggled_on):
	play_drums = toggled_on

func _on_song_toggle_toggled(toggled_on):
	play_song = toggled_on
	if not play_song:
		player.note_off_all(0)
