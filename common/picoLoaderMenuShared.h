#pragma once

/// @file picoLoaderMenuShared.h
/// @brief Shared-memory contract between pico-loader (in-game menu patch) and
///        pico-launcher (boot-time relaunch handler). This file MUST stay
///        byte-identical between the pico-loader and pico-launcher repos.
///
/// The address sits inside the DS firmware shared-memory region at the top of
/// main RAM (see shared_memory_ntr_t in sharedMemory.h). The region is
/// preserved across the launcher .nds soft-reset that pico-loader performs
/// when the user triggers a reboot/exit hotkey from inside a running ROM.

#define PLOAD_MENU_RELAUNCH_MAGIC_ADDR      (*(vu32*)0x02FFFFB0)

/// @brief Written by the in-game menu patch (in pico-loader) before triggering
///        the launcher soft-reset. Read and cleared by the launcher very early
///        in main() before any further memory is touched.
///
/// When this magic value is present, the launcher should skip its normal UI
/// and immediately re-launch the file at AppSettings.lastUsedFilePath. Any
/// other value means a normal launcher boot (clear by writing zero).
#define PLOAD_MENU_RELAUNCH_MAGIC           0xAB12C3D4u
