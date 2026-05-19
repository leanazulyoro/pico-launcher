#pragma once
#include "picoLoader7.h"

pload_params_t* pload_getLoadParams();
void pload_setBootDrive(PicoLoaderBootDrive bootDrive);
void pload_setLauncherPath(const char* launcherPath);
void pload_setCheatData(const pload_cheats_t* cheatData);
void pload_setMenuConfig(const pload_header7_v4_t& menuConfig);
void pload_start();
