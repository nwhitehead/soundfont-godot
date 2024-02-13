extends Node2D

# Called when the node enters the scene tree for the first time.
func _ready():
	var sf = SoundFont.new()
	# var file = FileAccess.open('res://example_broken.sf2', FileAccess.READ)
	var file = FileAccess.open('res://example.sf2', FileAccess.READ)
	var data = file.get_buffer(file.get_length())
	sf.set_data(data)
	print(sf.get_data())
	print(sf.get_first())
	$SoundFontGenerator.set_soundfont(sf)
	print($SoundFontGenerator.get_presetcount())
	print($SoundFontGenerator.get_presetname(0))


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
