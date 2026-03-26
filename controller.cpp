#include "controller.h"
#include "config.h"


namespace Steinberg {
namespace Vst {


FUnknown* VelocityController::createInstance(void*){
    return (IEditController*)new VelocityController();
}

VelocityController::VelocityController(): velocityGUIEditor(nullptr){
}

tresult PLUGIN_API VelocityController::initialize(FUnknown* context){
    tresult result = EditController::initialize(context);
    if(result != kResultTrue){
        return result;
    }

    StringListParameter* correctType = new StringListParameter(
        STR16("Correct Type"),  // title
        PARAM_ID_CORRECT_TYPE  // tag
    );
    correctType->appendString(STR16("Through"));
    correctType->appendString(STR16("Fix"));
    correctType->appendString(STR16("Remap"));
    correctType->appendString(STR16("Clip"));
    parameters.addParameter(correctType);

    VelocityStepParameter* velocityFix = new VelocityStepParameter(
        STR16("Velocity Fix"),  // title
        PARAM_ID_VELOCITY_FIX  // tag
    );
    parameters.addParameter(velocityFix);

    VelocityStepParameter* velocityMin = new VelocityStepParameter(
        STR16("Velocity Min"),  // title
        PARAM_ID_VELOCITY_MIN  // tag
    );
    parameters.addParameter(velocityMin);

    VelocityStepParameter* velocityMax = new VelocityStepParameter(
        STR16("Velocity Max"),  // title
        PARAM_ID_VELOCITY_MAX  // tag
    );
    parameters.addParameter(velocityMax);

    VelocityStepParameter* inputVelocity = new VelocityStepParameter(
        STR16("Input Velocity"),  // title
        PARAM_ID_INPUT_VELOCITY,  // tag
        N_VELOCITY_STEPS-1,  // step count
        DEFAULT_VELOCITY,  // default value plain
        MIN_VELOCITY,  // min plain
        MAX_VELOCITY,  // max plain
        ParameterInfo::kIsReadOnly|ParameterInfo::kIsHidden  // flags
    );
    parameters.addParameter(inputVelocity);

    VelocityStepParameter* outputVelocity = new VelocityStepParameter(
        STR16("Output Velocity"),  // title
        PARAM_ID_OUTPUT_VELOCITY,  // tag
        N_VELOCITY_STEPS-1,  // step count
        DEFAULT_VELOCITY,  // default value plain
        MIN_VELOCITY,  // min plain
        MAX_VELOCITY,  // max plain
        ParameterInfo::kIsReadOnly|ParameterInfo::kIsHidden  // flags
    );
    parameters.addParameter(outputVelocity);


    return result;
}

tresult PLUGIN_API VelocityController::setParamNormalized(ParamID tag, ParamValue value){
    EditController::setParamNormalized(tag, value);
    if(this->velocityGUIEditor == nullptr){  // in case GUI is closed
        return kResultOk;
    }

    switch(tag){
        case PARAM_ID_INPUT_VELOCITY:
            this->velocityGUIEditor->updateVelocityLabel(value, true);
            break;
        case PARAM_ID_OUTPUT_VELOCITY:
            this->velocityGUIEditor->updateVelocityLabel(value, false);
            break;
        default:
            // do nothing
            break;
    }

    return kResultOk;
}

IPlugView* PLUGIN_API VelocityController::createView(const char* name){
    if(strcmp(name, "editor") == 0){
        this->velocityGUIEditor = new VelocityGUIEditor(this);
        return this->velocityGUIEditor;
    }
    return 0;
}

void VelocityController::closeView(void){
    this->velocityGUIEditor = nullptr;
}


} }