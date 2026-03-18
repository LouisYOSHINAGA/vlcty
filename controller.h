#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "param.h"
#include "guieditor.h"


namespace Steinberg {
namespace Vst {


class VelocityStepParameter: public RangeParameter {
    public:
        VelocityStepParameter(
            const TChar* title, ParamID tag,
            int32 stepCount = N_VELOCITY_STEPS-1, ParamValue defaultValuePlain = DEFAULT_VELOCITY,
            ParamValue minPlain = MIN_VELOCITY, ParamValue maxPlain = MAX_VELOCITY,
            int32 flags = ParameterInfo::kCanAutomate, const TChar* units = nullptr, UnitID unitID = kRootUnitId
        ): RangeParameter(title, tag, units, minPlain, maxPlain, defaultValuePlain, stepCount, flags, unitID){
            this->setPrecision(0);
        }
};


class VelocityController: public EditController {
    protected:
        VelocityGUIEditor* velocityGUIEditor;
    public:
        static FUnknown* createInstance(void*);
        VelocityController();
        tresult PLUGIN_API initialize(FUnknown*);
        tresult PLUGIN_API setParamNormalized(ParamID, ParamValue);
        IPlugView* PLUGIN_API createView(const char*);
};


} }