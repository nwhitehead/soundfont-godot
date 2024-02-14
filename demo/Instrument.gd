extends AudioStreamPlayer

var player: SoundFontPlayer = null
func _ready():
	player = get_stream()
	print(player.get_presetname(0))

func _process(delta):
	print(delta)
