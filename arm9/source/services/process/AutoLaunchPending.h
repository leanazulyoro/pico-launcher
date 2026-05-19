#pragma once

/// @brief Set true by main() when the in-game-menu relaunch magic was present
///        at boot. Read by the App on startup to skip the normal UI and
///        immediately re-launch \see AppSettings.lastUsedFilePath.
inline bool gAutoLaunchPending = false;
