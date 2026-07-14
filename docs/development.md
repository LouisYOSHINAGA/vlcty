# Developer Documentation

This document describes the internal structure of VlctyCrrctr for anyone who
wants to read, modify, or build the code.

## Source layout

| File | Role |
| --- | --- |
| `config.h` / `config.cpp` | Plugin metadata (vendor, name, version), class UIDs, and the VST3 class factory. |
| `param.h` | Shared constants: parameter IDs, correction-type IDs, velocity ranges. |
| `processor.h` / `processor.cpp` | `VelocityProcessor` — the audio/event processor (real-time part). |
| `controller.h` / `controller.cpp` | `VelocityController` — the edit controller (parameter management, GUI factory). |
| `guieditor.h` / `guieditor.cpp` | `VelocityGUIEditor` — the VSTGUI-based editor. |
| `resource.rc` | Windows resource script embedding the PNGs in `resources/` into the binary. |
| `resources/` | GUI bitmap assets (background, slider track, slider handle). |

## Architecture

The plugin follows the standard VST3 split between processor and controller:

```
DAW (MIDI events, parameter automation)
        │
        ▼
VelocityProcessor ──(hidden IN/OUT velocity params via outputParameterChanges)──▶ DAW
   │  applyVelocityFix() rewrites note-on velocities                                │
   ▼                                                                                ▼
DAW (corrected events)                                            VelocityController.setParamNormalized()
                                                                                    │
                                                                                    ▼
                                                                  VelocityGUIEditor (label / control updates)
```

- **`VelocityProcessor`** receives note events, rewrites note-on velocities in
  `applyVelocityFix()` according to the current mode, and forwards all events.
  It also reports the last input/output velocity to the controller through the
  hidden read-only parameters `PARAM_ID_INPUT_VELOCITY` /
  `PARAM_ID_OUTPUT_VELOCITY` (the standard VST3 mechanism for
  processor→controller communication, like a VU meter).
  The stereo audio output declared in `initialize()` is a dummy that exists
  only so hosts treat the plugin as an instrument; `clearAudioOutput()` keeps
  it silent every block.
- **`VelocityController`** owns the parameter objects. Its
  `setParamNormalized()` override forwards every parameter change to the open
  editor: velocity monitor updates go to `updateVelocityLabel()`, everything
  else to `updateControl()`. This keeps the GUI in sync with host automation
  and preset loads, not just with direct user interaction.
- **`VelocityGUIEditor`** builds the frame in `open()`. User interaction goes
  through `valueChanged()`, which routes the new value through
  `VelocityController::setParamNormalized()` (syncing all related controls)
  and `performEdit()` (notifying the host). The editor registers itself with
  the controller on creation and unregisters in `close()` and in the
  destructor, so the controller never holds a dangling editor pointer.

## Parameters

All parameters use normalized values (0.0–1.0) on the VST3 side; velocities are
discretized to 128 steps (`value * MAX_VELOCITY + EPSILON`, truncated).

| ID | Name | Type | Notes |
| --- | --- | --- | --- |
| `PARAM_ID_CORRECT_TYPE` (0) | Correct Type | list (4 entries) | Through / Fix / Remap / Clip |
| `PARAM_ID_VELOCITY_FIX` (1) | Velocity Fix | 0–127, 127 steps | |
| `PARAM_ID_VELOCITY_MIN` (2) | Velocity Min | 0–127, 127 steps | |
| `PARAM_ID_VELOCITY_MAX` (3) | Velocity Max | 0–127, 127 steps | |
| `PARAM_ID_INPUT_VELOCITY` (4) | Input Velocity | read-only, hidden | monitor display only |
| `PARAM_ID_OUTPUT_VELOCITY` (5) | Output Velocity | read-only, hidden | monitor display only |

## State format

`VelocityProcessor::getState()` writes, and both
`VelocityProcessor::setState()` and `VelocityController::setComponentState()`
read, the following little-endian stream:

| Offset | Type | Field |
| --- | --- | --- |
| 0 | `int32` | correct type (`CorrectTypeID`, 0–3) |
| 4 | `float` | fix velocity (normalized 0.0–1.0) |
| 8 | `float` | min velocity (normalized 0.0–1.0) |
| 12 | `float` | max velocity (normalized 0.0–1.0) |

When changing this format, keep backward compatibility in mind: previously
saved DAW projects will feed the old stream into `setState()` /
`setComponentState()`. Add new fields at the end and treat a short stream as
"defaults for the missing fields" if compatibility is required.

## Building

Windows / Visual Studio against the
[Steinberg VST 3 SDK](https://github.com/steinbergmedia/vst3sdk) with the
bundled VSTGUI (`vstgui4`). Project files (`.sln` / `.vcxproj`) are gitignored;
add the four `.cpp` files plus `resource.rc` to a VST3 plugin project, compile
as C++17, and link the SDK's `base`, `sdk`, and `vstgui` libraries.

A quick header-level sanity check is possible on any platform without a full
build:

```sh
g++ -fsyntax-only -std=c++17 -w -I<vst3sdk> -I<vst3sdk>/vstgui4 \
    processor.cpp controller.cpp guieditor.cpp config.cpp
```

(Newer VSTGUI revisions than the one used for release builds may flag minor
API differences, e.g. the `CSlider::setStyle()` argument type.)

### Debug helper

Defining `DEBUG_GUIEDITOR` (see `guieditor.h`) draws white frames around
labels and text fields to make layout work easier.

## Coding conventions

The codebase intentionally deviates from common C++ style in a few ways; keep
these conventions when contributing:

- **Explicit `this->`** for member access (Java-style). This makes it obvious
  at the call site whether a name is a member or a local.
- 4-space indentation; `}else{` / `){` compact brace style; `switch` cases
  always carry a `break` and a `default` branch (with a `// do nothing`
  comment when intentionally empty).
- Constants are `UPPER_SNAKE_CASE` enums/`constexpr` in `param.h`; GUI-layout
  constants are `static constexpr` members of the editor class.
- Real-time code (`process()` and everything it calls) must not allocate,
  lock, or block.
