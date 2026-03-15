#pragma once

#include "pluginterfaces/base/ftypes.h"
#include "vstgui/lib/vstguibase.h"
#include <vector>


typedef enum{
    PARAM_ID_TYPE,
    PARAM_ID_VELOCITY_FIX,
    PARAM_ID_VELOCITY_MIN,
    PARAM_ID_VELOCITY_MAX,
    N_PARAMS
} ParamID;


typedef enum{
    CORRECT_TYPE_FIX,
    CORRECT_TYPE_REMAP,
    CORRECT_TYPE_CLIP,
    N_CORRECT_TYPES
} CorrectTypeID;

inline std::vector<VSTGUI::UTF8StringPtr> correctTypeNames = {
    "Fix",
    "Remap",
    "Clip"
};


constexpr Steinberg::uint8 N_VELOCITY_STEPS = 128;
constexpr Steinberg::uint8 MIN_VELOCITY = 0;
constexpr Steinberg::uint8 MAX_VELOCITY = N_VELOCITY_STEPS - 1;
constexpr Steinberg::uint8 DEFAULT_VELOCITY = 64;