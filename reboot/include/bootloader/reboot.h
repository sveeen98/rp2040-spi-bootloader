/**
 * @file reboot.h
 * @brief Reboot the RP2040 into the bootloader.
 * 
 * This project is a fork of Brian Starkey's RP2040 Serial Bootloader. The original project can be found at 
 * https://github.com/usedbytes/rp2040-serial-bootloader.git
 *
 *
 * @author Sven Koch
 * @date 2025
 * @license BSD-3-Clause
 */
#ifndef _BOOTLOADER_REBOOT_H_
#define _BOOTLOADER_REBOOT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

// Magic value for the watchdog scratch register to indicate 
// that the bootloader should be entered on the next reset.
#define BOOTLOADER_ENTRY_MAGIC 0xb105f00d

void bootloader_reboot(bool);

#ifdef __cplusplus
}
#endif

#endif /* _BOOTLOADER_REBOOT_H_ */
