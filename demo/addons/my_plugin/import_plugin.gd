@tool
extends EditorImportPlugin

func _get_importer_name():
	return "soundfont2"

func _get_visible_name():
	return "SoundFont2"

func _get_recognized_extensions():
	return ["sf2"]

func _get_save_extension():
	return "tres"

func _get_resource_type():
	return "SoundFont"

func _get_preset_count():
	return 1

func _get_preset_name(preset_index):
	return "Default"

func _get_import_options(path, preset_index):
	return []

func _get_priority():
	return 2

func _get_import_order():
	return 2

func _import(source_file, save_path, options, r_platform_variants, r_gen_files):
	var file = FileAccess.open(source_file, FileAccess.READ)
	if file == null:
		return FileAccess.get_open_error()
	var data = file.get_buffer(file.get_length())
	var sf = SoundFont.new()
	sf.set_data(data)
	return ResourceSaver.save(sf, "%s.%s" % [save_path, _get_save_extension()])
