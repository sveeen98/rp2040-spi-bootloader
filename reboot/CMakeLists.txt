
add_library(bootloader_reboot INTERFACE)

target_include_directories(bootloader_reboot INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(bootloader_reboot INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/reboot.c
)

target_link_libraries(bootloader_reboot INTERFACE
	cmsis_core
	hardware_structs
)

