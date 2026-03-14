#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"


namespace Steinberg {
namespace Vst {


constexpr int8 NUM_EVENT_CHANNELS = 1;


class VelocityProcessor: public AudioEffect {
    private:
       float velocity;
    public:
        static FUnknown* createInstance(void*);
        VelocityProcessor();
        tresult PLUGIN_API initialize(FUnknown*);
        tresult PLUGIN_API setBusArrangements(SpeakerArrangement*, int32, SpeakerArrangement*, int32);
        tresult PLUGIN_API process(ProcessData&);
        virtual void processParameter(ProcessData&);
        virtual void processEvent(IEventList*, IEventList*);
        virtual void applyVelocityFix(NoteOnEvent*);
};


} }