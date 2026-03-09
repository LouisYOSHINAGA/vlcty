#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "const.h"


#define NUM_EVENT_CHANNELS 1


using namespace std;
namespace Steinberg {
namespace Vst {


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