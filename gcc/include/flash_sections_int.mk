_intflash_:
	@cd "$(st_stm32cube_programmer_path)" && ./$(stm32cube_programmer_filename) -c port=SWD -d $(application_path)/$(binary_output_path)/intflash.hex -hardRst
