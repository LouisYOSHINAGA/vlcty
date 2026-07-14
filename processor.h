#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "param.h"


namespace Steinberg {
namespace Vst {


constexpr int8 NUM_EVENT_CHANNELS = 1;


class VelocityProcessor: public AudioEffect {
    protected:
        CorrectTypeID correctType;
        float fixVelocity;
        float minVelocity;
        float maxVelocity;
        float inputVelocity;
        float outputVelocity;
        void processParameter(ProcessData&);
        void processEvent(ProcessData&);
        void applyVelocityFix(NoteOnEvent&);
        void sendParameterToController(ParamID, float, IParameterChanges&, int32);
        void clearAudioOutput(ProcessData&);
    public:
        static FUnknown* createInstance(void*);
        VelocityProcessor();
        tresult PLUGIN_API initialize(FUnknown*);
        tresult PLUGIN_API setBusArrangements(SpeakerArrangement*, int32, SpeakerArrangement*, int32);
        tresult PLUGIN_API process(ProcessData&);
        tresult PLUGIN_API setState(IBStream*);
        tresult PLUGIN_API getState(IBStream*);
};


} }