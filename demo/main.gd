extends Node2D

var play_arp: bool = false
var play_song: bool = false
var preset_arp: int = 5
var preset_song: int = 7
var player: SoundFontPlayer = null
var base_note: int = 48
var arpnotes = [0, 4, 7, 12]
var arpseq = [0, 1, 2, 3, 2, 1, 0, 1]
var arppos = 0
var oldtime = 0

var time = 0
var old_ptime = 0
var song
var song_time = 0
var song_position = 0
var current_bpm = 115.0
var current_beat = 0
var song_offset = 0

func _ready():
	song = ResourceLoader.load('res://1080-c01.mid')
	player = $SoundFontPlayer
	var num_presets = player.get_presetcount()
	print('SF2 num preset=', num_presets)
	for i in range(num_presets):
		print(i, ' - ', player.get_presetname(i))
	# Set instrument for arp
	player.channel_set_presetindex(0, 2, preset_arp)
	# Set instrument for midi song
	player.channel_set_presetindex(0, 3, preset_song)
	player.channel_set_presetindex(0, 4, preset_song)
	player.channel_set_presetindex(0, 5, preset_song)
	player.channel_set_presetindex(0, 6, preset_song)

func _process(delta):
	var ptime = player.get_time()
	var delta_ptime = ptime - old_ptime
	current_beat += delta_ptime * current_bpm / 60
	var beat_subdivision : int = 8
	var current_beat_i : int = int(current_beat)
	var current_beat_frac : int = int((current_beat - current_beat_i) * beat_subdivision)
	$Beat.text = '%d  %d / %d' % [current_beat_i, current_beat_frac, beat_subdivision]
	old_ptime = ptime
	var dur = 60.0 / current_bpm / 4.0
	var end_time = ptime + 1.0 / Engine.get_physics_ticks_per_second() + 0.1
	while time < end_time:
		if play_arp:
			var note = base_note + arpnotes[arpseq[arppos]]
			player.channel_note_on(time, 2, note, 0.5)
			player.channel_note_off(time + dur, 2, note)
		time += dur
		arppos += 1
		if arppos >= len(arpseq):
			arppos = 0			

	while song_position < song.get_events().size() and song.get_events()[song_position]['t'] + song_offset < end_time:
		var event = song.get_events()[song_position]
		var t = event['t'] + song_offset
		var type = event['type']
		var channel = event['channel']
		var key = event['key']
		var vel = event['velocity'] / 255.0
		current_bpm = event['bpm']
		$CurrentBPM.text = '%d' % current_bpm
		song_position += 1
		if type == Midi.NOTE_ON:
			if play_song:
				player.channel_note_on(t, 3 + channel, key, vel)
	if song_position == song.get_events().size() and song_position > 0:
		player.note_off_all(song.get_events()[song_position - 1][0] + song_offset)

func _on_button_button_down_c():
	# D F A D
	player.channel_set_pan(0, 2, 0)
	arpnotes = [2, 5, 9, 14]

func _on_button_button_down_g():
	# A C# E G
	player.channel_set_pan(0, 2, 1.0)
	arpnotes = [-3, 1, 4, 7]

func _on_arp_toggle_toggled(toggled_on):
	play_arp = toggled_on

func _on_song_toggle_toggled(toggled_on):
	play_song = toggled_on
	if play_song:
		song_position = 0
		song_offset = player.get_time()
		current_beat = 0
	if not play_song:
		player.note_off_all(0)

func _input(event):
	var map = {
		"note_c":0,
		"note_d":2,
		"note_e":4,
		"note_f":5,
		"note_g":7,
		"note_a":9,
		"note_b":11,
		"note_c2":12,
		"note_d2":14,
	}
	var base_note : int = 53 # F
	for k in map:
		if event.is_action_pressed(k):
			player.channel_note_on(0, 0, base_note + map[k], 1.0)
		if event.is_action_released(k):
			player.channel_note_off(0, 0, base_note + map[k])
