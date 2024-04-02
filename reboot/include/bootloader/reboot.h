/**
 * Copyright (c) 2022 Brian Starkey <stark3y@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOOTLOADER_REBOOT_H_
#define _BOOTLOADER_REBOOT_H_

#include <stdbool.h>

// The bootloader can be entered in three ways:
//  - BOOTLOADER_ENTRY_PIN is low
//  - Watchdog scratch[5] == BOOTLOADER_ENTRY_MAGIC && scratch[6] == ~BOOTLOADER_ENTRY_MAGIC
//  - No valid image header
#define BOOTLOADER_ENTRY_MAGIC 0xb105f00d

void bootloader_reboot(bool to_bootloader);

#endif /* _BOOTLOADER_REBOOT_H_ */
