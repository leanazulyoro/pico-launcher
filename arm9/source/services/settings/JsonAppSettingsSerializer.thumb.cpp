#include "common.h"
#include <memory>
#include "json/ArduinoJson.h"
#include "AppSettings.h"
#include "fat/File.h"
#include "JsonAppSettingsSerializer.h"

#pragma GCC optimize("Os")

#define JSON_RESERVED_SIZE  2048

#define KEY_LANGUAGE                 "language"
#define KEY_ROM_BROWSER_LAYOUT       "romBrowserLayout"
#define KEY_ROM_BROWSER_SORT_MODE    "romBrowserSortMode"
#define KEY_THEME                    "theme"
#define KEY_LAST_USED_FILE_PATH      "lastUsedFilePath"
#define KEY_FILE_ASSOCIATIONS        "fileAssociations"
#define KEY_FILE_ASSOCIATIONS_APPLICATION_PATH  "appPath"
#define KEY_IN_GAME_MENU_ENABLED        "inGameMenuEnabled"
#define KEY_IN_GAME_EXIT_HOTKEY         "inGameExitHotkey"
#define KEY_IN_GAME_REBOOT_HOTKEY       "inGameRebootHotkey"
#define KEY_IN_GAME_BRIGHTNESS_HOTKEY   "inGameBrightnessHotkey"
#define KEY_IN_GAME_CLOCK_HOTKEY        "inGameClockHotkey"
#define KEY_DEFAULT_BRIGHTNESS          "defaultBrightness"

static const char* serializeRomBrowserLayout(RomBrowserLayout romBrowserLayout)
{
    switch (romBrowserLayout)
    {
        case RomBrowserLayout::HorizontalIconGrid:
            return "HorizontalIconGrid";
        case RomBrowserLayout::VerticalIconGrid:
            return "VerticalIconGrid";
        case RomBrowserLayout::BannerList:
            return "BannerList";
        case RomBrowserLayout::FileList:
            return "FileList";
        case RomBrowserLayout::CoverFlow:
            return "CoverFlow";
        default:
            return "";
    }
}

static bool tryParseRomBrowserLayout(
    const char* romBrowserLayoutString, RomBrowserLayout& romBrowserLayout)
{
    if (!romBrowserLayoutString)
        return false;

    if (!strcasecmp(romBrowserLayoutString, "HorizontalIconGrid"))
        romBrowserLayout = RomBrowserLayout::HorizontalIconGrid;
    else if (!strcasecmp(romBrowserLayoutString, "VerticalIconGrid"))
        romBrowserLayout = RomBrowserLayout::VerticalIconGrid;
    else if (!strcasecmp(romBrowserLayoutString, "BannerList"))
        romBrowserLayout = RomBrowserLayout::BannerList;
    else if (!strcasecmp(romBrowserLayoutString, "FileList"))
        romBrowserLayout = RomBrowserLayout::FileList;
    else if (!strcasecmp(romBrowserLayoutString, "CoverFlow"))
        romBrowserLayout = RomBrowserLayout::CoverFlow;
    else
        return false;

    return true;
}

static const char* serializeRomBrowserSortMode(RomBrowserSortMode romBrowserSortMode)
{
    switch (romBrowserSortMode)
    {
        case RomBrowserSortMode::NameAscending:
            return "NameAscending";
        case RomBrowserSortMode::NameDescending:
            return "NameDescending";
        case RomBrowserSortMode::LastModified:
            return "LastModified";
        default:
            return "";
    }
}

static bool tryParseRomBrowserSortMode(
    const char* romBrowserDisplayModeString, RomBrowserSortMode& romBrowserSortMode)
{
    if (!romBrowserDisplayModeString)
        return false;

    if (!strcasecmp(romBrowserDisplayModeString, "NameAscending"))
        romBrowserSortMode = RomBrowserSortMode::NameAscending;
    else if (!strcasecmp(romBrowserDisplayModeString, "NameDescending"))
        romBrowserSortMode = RomBrowserSortMode::NameDescending;
    else if (!strcasecmp(romBrowserDisplayModeString, "LastModified"))
        romBrowserSortMode = RomBrowserSortMode::LastModified;
    else
        return false;

    return true;
}

static bool tryParseFileAssociations(const JsonObjectConst& json, AppSettings* appSettings)
{
    if (json.isNull())
    {
        return false;
    }

    appSettings->fileAssociations = std::make_unique_for_overwrite<FileAssociation[]>(json.size());
    int i = 0;
    for (auto item : json)
    {
        auto extension = item.key().c_str();
        auto appPath = item.value()[KEY_FILE_ASSOCIATIONS_APPLICATION_PATH].as<const char*>();
        appSettings->fileAssociations[i++] = FileAssociation(extension, appPath);
    }
    appSettings->numberOfFileAssociations = i;
    return true;
}

static void serializeFileAssociations(DynamicJsonDocument& json, const AppSettings* appSettings)
{
    auto jsonObject = json[KEY_FILE_ASSOCIATIONS].to<JsonObject>();
    for (u32 i = 0; i < appSettings->numberOfFileAssociations; i++)
    {
        const auto& fileAssociation = appSettings->fileAssociations[i];
        auto jsonAssociation = jsonObject[fileAssociation.extension.GetString()].to<JsonObject>();
        jsonAssociation[KEY_FILE_ASSOCIATIONS_APPLICATION_PATH] = fileAssociation.applicationPath.GetString();
    }
}

static const char* serializeInputKey(InputKey key)
{
    switch (key)
    {
        case InputKey::A:         return "A";
        case InputKey::B:         return "B";
        case InputKey::Select:    return "Select";
        case InputKey::Start:     return "Start";
        case InputKey::DpadRight: return "DpadRight";
        case InputKey::DpadLeft:  return "DpadLeft";
        case InputKey::DpadUp:    return "DpadUp";
        case InputKey::DpadDown:  return "DpadDown";
        case InputKey::R:         return "R";
        case InputKey::L:         return "L";
        case InputKey::X:         return "X";
        case InputKey::Y:         return "Y";
        default:                  return nullptr;
    }
}

static bool tryParseInputKey(const char* s, InputKey& key)
{
    if (!s)
        return false;
    if (!strcasecmp(s, "A"))              key = InputKey::A;
    else if (!strcasecmp(s, "B"))         key = InputKey::B;
    else if (!strcasecmp(s, "Select"))    key = InputKey::Select;
    else if (!strcasecmp(s, "Start"))     key = InputKey::Start;
    else if (!strcasecmp(s, "DpadRight")) key = InputKey::DpadRight;
    else if (!strcasecmp(s, "DpadLeft"))  key = InputKey::DpadLeft;
    else if (!strcasecmp(s, "DpadUp"))    key = InputKey::DpadUp;
    else if (!strcasecmp(s, "DpadDown"))  key = InputKey::DpadDown;
    else if (!strcasecmp(s, "R"))         key = InputKey::R;
    else if (!strcasecmp(s, "L"))         key = InputKey::L;
    else if (!strcasecmp(s, "X"))         key = InputKey::X;
    else if (!strcasecmp(s, "Y"))         key = InputKey::Y;
    else return false;
    return true;
}

static void serializeInputKeyMask(DynamicJsonDocument& json, const char* key, InputKey mask)
{
    auto arr = json[key].to<JsonArray>();
    for (u16 i = 0; i < 16; i++)
    {
        auto bit = static_cast<InputKey>(1u << i);
        if ((mask & bit) == InputKey::None)
            continue;
        const char* name = serializeInputKey(bit);
        if (name)
            arr.add(name);
    }
}

static bool tryParseInputKeyMask(JsonVariantConst json, InputKey& mask)
{
    if (!json.is<JsonArrayConst>())
        return false;
    InputKey result = InputKey::None;
    for (auto element : json.as<JsonArrayConst>())
    {
        InputKey key;
        if (!tryParseInputKey(element.as<const char*>(), key))
            return false;
        result |= key;
    }
    mask = result;
    return true;
}

static std::unique_ptr<u8[]> writeJson(const AppSettings* appSettings, u32& length)
{
    DynamicJsonDocument json(JSON_RESERVED_SIZE);
    json[KEY_LANGUAGE] = appSettings->language.GetString();
    json[KEY_ROM_BROWSER_LAYOUT] = serializeRomBrowserLayout(appSettings->romBrowserDisplaySettings.layout);
    json[KEY_ROM_BROWSER_SORT_MODE] = serializeRomBrowserSortMode(appSettings->romBrowserDisplaySettings.sortMode);
    json[KEY_THEME] = appSettings->theme.GetString();
    json[KEY_LAST_USED_FILE_PATH] = appSettings->lastUsedFilePath.GetString();
    serializeFileAssociations(json, appSettings);

    json[KEY_IN_GAME_MENU_ENABLED] = appSettings->inGameMenuEnabled;
    json[KEY_DEFAULT_BRIGHTNESS] = appSettings->defaultBrightness;
    serializeInputKeyMask(json, KEY_IN_GAME_EXIT_HOTKEY,       appSettings->inGameExitHotkey);
    serializeInputKeyMask(json, KEY_IN_GAME_REBOOT_HOTKEY,     appSettings->inGameRebootHotkey);
    serializeInputKeyMask(json, KEY_IN_GAME_BRIGHTNESS_HOTKEY, appSettings->inGameBrightnessHotkey);
    serializeInputKeyMask(json, KEY_IN_GAME_CLOCK_HOTKEY,      appSettings->inGameClockHotkey);

    u32 outputSize = measureJsonPretty(json);
    std::unique_ptr<u8[]> fileData(new(cache_align) u8[outputSize]);

    serializeJsonPretty(json, fileData.get(), outputSize);

    length = outputSize;
    return fileData;
}

void JsonAppSettingsSerializer::Serialize(const AppSettings* appSettings, const char* filePath) const
{
    u32 length = 0;
    std::unique_ptr<u8[]> fileData = writeJson(appSettings, length);

    const auto file = std::make_unique<File>();
    if (file->Open(filePath, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
    {
        LOG_ERROR("Couldn't open settings file for writing\n");
        return;
    }

    u32 bytesWritten;
    if (file->Write(fileData.get(), length, bytesWritten) != FR_OK || bytesWritten != length)
    {
        LOG_ERROR("Error while writing settings file\n");
        return;
    }

    LOG_DEBUG("Settings file written\n");
}

static void readJson(AppSettings* appSettings, const JsonDocument& json)
{
    appSettings->language = json[KEY_LANGUAGE] | appSettings->language.GetString();
    appSettings->theme = json[KEY_THEME] | appSettings->theme.GetString();
    appSettings->lastUsedFilePath = json[KEY_LAST_USED_FILE_PATH] | appSettings->lastUsedFilePath.GetString();

    RomBrowserLayout romBrowserLayout;
    if (tryParseRomBrowserLayout(json[KEY_ROM_BROWSER_LAYOUT].as<const char*>(),
            romBrowserLayout))
    {
        appSettings->romBrowserDisplaySettings.layout = romBrowserLayout;
    }
    RomBrowserSortMode romBrowserSortMode;
    if (tryParseRomBrowserSortMode(json[KEY_ROM_BROWSER_SORT_MODE].as<const char*>(),
            romBrowserSortMode))
    {
        appSettings->romBrowserDisplaySettings.sortMode = romBrowserSortMode;
    }

    tryParseFileAssociations(json[KEY_FILE_ASSOCIATIONS], appSettings);

    appSettings->inGameMenuEnabled = json[KEY_IN_GAME_MENU_ENABLED] | appSettings->inGameMenuEnabled;
    appSettings->defaultBrightness = json[KEY_DEFAULT_BRIGHTNESS] | appSettings->defaultBrightness;

    InputKey hotkey;
    if (tryParseInputKeyMask(json[KEY_IN_GAME_EXIT_HOTKEY], hotkey))
        appSettings->inGameExitHotkey = hotkey;
    if (tryParseInputKeyMask(json[KEY_IN_GAME_REBOOT_HOTKEY], hotkey))
        appSettings->inGameRebootHotkey = hotkey;
    if (tryParseInputKeyMask(json[KEY_IN_GAME_BRIGHTNESS_HOTKEY], hotkey))
        appSettings->inGameBrightnessHotkey = hotkey;
    if (tryParseInputKeyMask(json[KEY_IN_GAME_CLOCK_HOTKEY], hotkey))
        appSettings->inGameClockHotkey = hotkey;
}

bool JsonAppSettingsSerializer::Deserialize(AppSettings* appSettings, const char* filePath) const
{
    const auto file = std::make_unique<File>();
    if (file->Open(filePath, FA_READ | FA_OPEN_EXISTING) != FR_OK)
        return false;

    u32 fileSize = file->GetSize();
    if (fileSize == 0)
        return false;

    std::unique_ptr<u8[]> fileData(new(cache_align) u8[fileSize]);
    u8* fileDataPtr = fileData.get();

    u32 bytesRead = 0;
    if (file->Read(fileDataPtr, fileSize, bytesRead) != FR_OK)
        return false;

    DynamicJsonDocument json(JSON_RESERVED_SIZE);
    if (deserializeJson(json, fileDataPtr, fileSize) != DeserializationError::Ok)
        return false;

    readJson(appSettings, json);

    return true;
}