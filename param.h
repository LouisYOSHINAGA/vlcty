#pragma once

#include "pluginterfaces/base/ftypes.h"
#include <vector>


typedef enum{
    PARAM_ID_TYPE,
    PARAM_ID_VELOCITY,
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


constexpr Steinberg::int8 DEFAULT_VELOCITY = 64;