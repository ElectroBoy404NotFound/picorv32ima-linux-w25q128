add_executable(pico-rv32ima
    main.c
	sd_hw_config.c
	ram.c
	emulator.c
	usb_descriptors.c
    cdc_console.c
	terminal.c
	cache.c
)

# Add the standard library and FatFS/SPI to the build
target_link_libraries(pico-rv32ima
	pico_stdlib 
	FatFs_SPI 
	pico_time
	pico_multicore
	hardware_vreg
	tinyusb_device 
	tinyusb_board
	st7735
	gfx
)

target_include_directories(pico-rv32ima PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tusb_inc )

# pico_set_binary_type(pico-rv32ima copy_to_ram)
pico_add_extra_outputs(pico-rv32ima)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)
set(PICO_CXX_ENABLE_RTTI 1)
