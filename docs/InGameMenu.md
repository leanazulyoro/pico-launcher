# In-game Menu
The in-game menu lets you reboot the current game, return to Pico Launcher, change DS brightness, and see the time — from inside any running ROM, including retail games.

It's triggered by configurable button combos held for about half a second while a game is running.

## Requirements
- Pico Loader API v4 or newer (`picoLoader7.bin` + `picoLoader9.bin` from a build that supports it)
- The in-game menu binary `igm9.bin` present at `/_pico/igm9.bin` on your SD card

If these are missing the launcher still works, but the in-game menu won't appear when you press the hotkeys.

## Default hotkeys
Hold for ~0.5 seconds.

| Combo | Action |
|---|---|
| L + R + Start | Exit to Pico Launcher |
| L + R + Select | Reboot current game |
| L + R + Up | Cycle screen brightness |
| L + R + B | Show current time on the menu screen |

> [!WARNING]
> Rebooting or exiting from the in-game menu **loses unsaved game progress**. Save inside the game first.

## Settings
The in-game menu is enabled by default. To change behavior, edit `/_pico/settings.json`:

```json
{
  "inGameMenuEnabled": true,
  "defaultBrightness": 1,
  "inGameExitHotkey": ["L", "R", "Start"],
  "inGameRebootHotkey": ["L", "R", "Select"],
  "inGameBrightnessHotkey": ["L", "R", "Up"],
  "inGameClockHotkey": ["L", "R", "B"]
}
```

- `inGameMenuEnabled` — set to `false` to disable hotkey detection entirely
- `defaultBrightness` — initial level when a game launches (0..3)
- Each hotkey is an array of key names. Recognized names: `A`, `B`, `Select`, `Start`, `DpadRight`, `DpadLeft`, `DpadUp`, `DpadDown`, `L`, `R`, `X`, `Y` (case-insensitive)

### Hotkey caveat
`X`, `Y`, `Touch`, and `Lid` cannot be used in hotkey combos in this version — the in-game patch only reads the standard 10-button input register. Settings containing these keys will round-trip through JSON correctly but the affected bits are dropped before reaching the running game.

## Game compatibility
Reboot and exit work in any ROM. Resuming a game after the menu closes may fail on some titles — if it does, use Reboot to get back to a clean state. A list of known-incompatible games will be maintained in the project's issues.
