/**
 * @file reboot.c
 * @brief Reboot the RP2040 into the bootloader.
 * 
 * This project is a fork of Brian Starkey's RP2040 SPI Bootloader. The original project can be found at 
 * https://github.com/usedbytes/rp2040-serial-bootloader.git
 *
 *
 * @author Sven Koch
 * @date 2024
 * @license BSD-3-Clause
 */

#include "RP2040.h"
#include "hardware/structs/watchdog.h"
#include "hardware/watchdog.h"

#include "bootloader/reboot.h"

void bootloader_reboot(bool to_bootloader) {
	hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);

	if (to_bootloader) {
		watchdog_hw->scratch[5] = BOOTLOADER_ENTRY_MAGIC;
		watchdog_hw->scratch[6] = ~BOOTLOADER_ENTRY_MAGIC;
	} else {
		watchdog_hw->scratch[5] = 0;
		watchdog_hw->scratch[6] = 0;
	}

	watchdog_reboot(0, 0, 0);

	while (1) {
		tight_loop_contents();
	}
}
