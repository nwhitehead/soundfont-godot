extends Node2D

# Called when the node enters the scene tree for the first time.
func _ready():
	print(load('res://main.gd'))

	var file = FileAccess.open('res://example.sf2', FileAccess.READ)
	var text = file.get_buffer(file.get_length())
	print(text)
	print(text.get_class())
	print(load('res://example.sf2'))


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
