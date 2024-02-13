extends Node2D


# Called when the node enters the scene tree for the first time.
func _ready():
	var file = FileAccess.open('res://text.txt', FileAccess.READ)
	var text = file.get_as_text()
	print(text)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
