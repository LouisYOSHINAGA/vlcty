# vlcty — VlctyCrrctr

**VlctyCrrctr** is a lightweight VST3 plugin that corrects the velocity of incoming
MIDI notes in real time. Insert it in front of any instrument to fix, remap, or
clip note-on velocities without touching the recorded MIDI data.

> 日本語のユーザーマニュアルは [docs/manual.ja.md](docs/manual.ja.md) を参照してください。

## Features

- Four correction modes, switchable from a combobox on the GUI
- Velocity values adjustable by slider or by direct text input (0–127)
- Real-time input/output velocity monitor (`in -> out`)
- GUI zoom (100%–200%, in 25% steps) via right-click context menu
- Settings are saved and restored with your DAW project

## Correction modes

| Mode | Behavior |
| --- | --- |
| **Through** | Passes velocities through unchanged. |
| **Fix** | Forces every note-on velocity to the *Fix* value. |
| **Remap** | Linearly remaps the full input velocity range (1–127) onto the *Min*–*Max* range. |
| **Clip** | Clamps velocities into the *Min*–*Max* range. |

Notes on edge cases:

- A note-on with velocity 0 (the MIDI convention for note-off) is always passed
  through unmodified, regardless of the mode.
- In **Clip** mode, setting *Min* greater than *Max* is treated as an invalid
  configuration and mutes notes (outputs velocity 0). This can be used
  intentionally as a velocity gate.
- All other events (note-off, controllers, etc.) are forwarded untouched.

## Parameters

| Parameter | Range | Used by | Description |
| --- | --- | --- | --- |
| Correct Type | Through / Fix / Remap / Clip | — | Selects the correction mode. |
| Velocity Fix | 0–127 | Fix | The fixed velocity value. |
| Velocity Min | 0–127 | Remap, Clip | Lower bound of the target range. |
| Velocity Max | 0–127 | Remap, Clip | Upper bound of the target range. |

Sliders that are not used by the current mode are dimmed and locked.
The plugin also exposes two hidden read-only parameters (*Input Velocity* /
*Output Velocity*) which drive the on-screen velocity monitor.

## Usage

1. Copy the built `.vst3` bundle into `C:\Program Files\Common Files\VST3`.
2. In your DAW, insert **VlctyCrrctr** as an instrument. The plugin registers
   itself as an instrument with a silent dummy stereo output so that it can be
   placed in a MIDI signal chain.
3. Route your MIDI input into VlctyCrrctr, and route its MIDI output into the
   instrument you want to drive (the exact routing steps depend on your DAW).
4. Play notes — the monitor at the top right shows the last input and output
   velocities as `in -> out`.
5. Right-click anywhere on the plugin window to change the GUI zoom level.

## Building

The plugin currently targets **Windows** and is built with **Visual Studio**
against the [Steinberg VST 3 SDK](https://github.com/steinbergmedia/vst3sdk)
(which bundles VSTGUI). Project files are intentionally not tracked in this
repository; to build:

1. Get the VST 3 SDK (including the `vstgui4` submodule).
2. Create a VST3 plugin project (a DLL project renamed to `.vst3`, or a
   CMake-based project using the SDK's `smtg_add_vst3plugin` helper).
3. Add all `.cpp`/`.h` files in this repository plus `resource.rc` to the
   project. The `.rc` file embeds the PNG assets in `resources/` into the
   binary, where VSTGUI's `CBitmap` loads them by name.
4. Compile as C++17 or later, linking the SDK's `base`, `sdk`
   (public.sdk) and `vstgui` libraries, and export the standard VST3 module
   entry points (the class factory is defined in `config.cpp`).

Developer documentation — architecture, parameter/state formats, and the
project's coding conventions — lives in [docs/development.md](docs/development.md).

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).

VST is a trademark of Steinberg Media Technologies GmbH, registered in Europe
and other countries.
