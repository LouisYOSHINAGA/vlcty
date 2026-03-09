#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"


namespace Steinberg {
namespace Vst {


class DiscreteRangeParameter: public RangeParameter{
    public:
        DiscreteRangeParameter(
            const TChar* title, ParamID tag, const TChar* units=0, int32 stepCount=99,
            ParamValue minPlain=0, ParamValue maxPlain=99, ParamValue defaultValuePlain=0,
            int32 flags=ParameterInfo::kCanAutomate, UnitID unitID=kRootUnitId
        ): RangeParameter(title, tag, units, minPlain, maxPlain, defaultValuePlain,
                          stepCount, flags, unitID){
            this->setPrecision(0);
        }
};


class VelocityController: public EditController {
    public:
        static FUnknown* createInstance(void*);
        tresult PLUGIN_API initialize(FUnknown*);
};


} }