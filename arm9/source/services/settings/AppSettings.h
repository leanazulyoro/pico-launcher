#pragma once
#include <memory>
#include "core/String.h"
#include "gui/input/InputKey.h"
#include "RomBrowserDisplaySettings.h"
#include "FileAssociation.h"

class AppSettings
{
public:
    String<char, 16> language = "english";
    String<char, 64> theme = "material";
    String<char, 256> lastUsedFilePath = "";
    RomBrowserDisplaySettings romBrowserDisplaySettings;

    std::unique_ptr<FileAssociation[]> fileAssociations;
    u32 numberOfFileAssociations = 0;

    bool inGameMenuEnabled = true;
    InputKey inGameExitHotkey       = InputKey::L | InputKey::R | InputKey::Start;
    InputKey inGameRebootHotkey     = InputKey::L | InputKey::R | InputKey::Select;
    InputKey inGameBrightnessHotkey = InputKey::L | InputKey::R | InputKey::DpadUp;
    InputKey inGameClockHotkey      = InputKey::L | InputKey::R | InputKey::B;
    u8 defaultBrightness = 1;
};