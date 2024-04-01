# Derived from the Pico SDK, which carries the following
# LICENSE.txt:
# Copyright 2020 (c) 2020 Raspberry Pi (Trading) Ltd.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
# following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
#    disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "../pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rp2040-spi-bootloader C CXX ASM)

pico_sdk_init()

# Build the bootloader as a standalone thing

add_executable(rp2040-spi-bootloader main.c)

function(target_cl_options option)
    target_compile_options(rp2040-spi-bootloader PRIVATE ${option})
    target_link_options(rp2040-spi-bootloader PRIVATE ${option})
endfunction()

target_cl_options("-Wall")
target_cl_options("-Os")
target_cl_options("-ffunction-sections")
target_cl_options("-fdata-sections")
target_link_options(rp2040-spi-bootloader PRIVATE "LINKER:--gc-sections")

pico_add_extra_outputs(rp2040-spi-bootloader)

pico_enable_stdio_uart(rp2040-spi-bootloader 0)
pico_enable_stdio_usb(rp2040-spi-bootloader 1)

target_link_libraries(rp2040-spi-bootloader
                      pico_stdlib
                      hardware_dma
                      hardware_flash
					  hardware_spi
                      hardware_structs
                      hardware_resets
                      cmsis_core)

# Provide a helper to build a standalone target
function(bootloader_build_standalone NAME)
	get_target_property(BOOTLOADER_SRC_DIR rp2040-spi-bootloader SOURCE_DIR)
	pico_set_linker_script(${NAME} ${BOOTLOADER_SRC_DIR}/standalone.ld)
	pico_add_bin_output(${NAME})
endfunction()

# Provide a helper to build a combined target
function(bootloader_build_combined NAME)
	set(APP_BIN ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.bin)
	set(APP_HDR ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_hdr.bin)

	set(COMBINED ${NAME}_combined)

	get_target_property(BOOTLOADER_SRC_DIR rp2040-spi-bootloader SOURCE_DIR)
	get_target_property(BOOTLOADER_BIN_DIR rp2040-spi-bootloader BINARY_DIR)

	get_directory_property(initial_clean ADDITIONAL_MAKE_CLEAN_FILES)
	list(APPEND clean_files ${initial_clean})
	list(APPEND clean_files ${APP_BIN})
	list(APPEND clean_files ${APP_HDR})

	# The app must be built with the correct linker script (and a .bin)
	bootloader_build_standalone(${NAME})

	pico_set_linker_script(rp2040-spi-bootloader ${BOOTLOADER_SRC_DIR}/combined.ld)

	add_custom_target(${NAME}_hdr DEPENDS ${NAME} rp2040-spi-bootloader)
	add_custom_command(TARGET ${NAME}_hdr
		COMMAND ${BOOTLOADER_SRC_DIR}/gen_imghdr.py --map ${BOOTLOADER_BIN_DIR}/rp2040-spi-bootloader.elf.map --section .app_bin ${APP_BIN} ${APP_HDR}
	)

	add_custom_target(${COMBINED} ALL)
	add_dependencies(${COMBINED} rp2040-spi-bootloader ${NAME}_hdr)
	add_custom_command(TARGET ${COMBINED}
		COMMAND ${CMAKE_OBJCOPY}
			--update-section .app_hdr=${APP_HDR}
			--update-section .app_bin=${APP_BIN} ${BOOTLOADER_BIN_DIR}/rp2040-spi-bootloader.elf ${COMBINED}.elf
	)
	list(APPEND clean_files ${COMBINED}.bin)
	list(APPEND clean_files ${COMBINED}.elf)

	add_custom_command(TARGET ${COMBINED} POST_BUILD
		COMMAND ${CMAKE_OBJCOPY} -Obinary ${COMBINED}.elf ${COMBINED}.bin
	)

	if (NOT ELF2UF2_FOUND)
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PICO_SDK_PATH}/tools)
		find_package(ELF2UF2)
	endif()

	if (ELF2UF2_FOUND)
		add_custom_command(TARGET ${COMBINED} POST_BUILD
			COMMAND ELF2UF2 ${COMBINED}.elf ${COMBINED}.uf2
		)
		list(APPEND clean_files ${COMBINED}.uf2)
	endif()

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${clean_files}")
endfunction()
