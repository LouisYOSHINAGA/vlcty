#include "processor.h"
#include "config.h"
#include "const.h"


namespace Steinberg {
namespace Vst {


FUnknown* VelocityProcessor::createInstance(void*){
    return (IAudioProcessor*)new VelocityProcessor();
}

VelocityProcessor::VelocityProcessor(){
    setControllerClass(ControllerUID);
}

tresult PLUGIN_API VelocityProcessor::initialize(FUnknown* context){
    tresult result = AudioEffect::initialize(context);
    if(result == kResultTrue){
        addEventInput(STR16("EventInput"), NUM_EVENT_CHANNELS);
        addEventOutput(STR16("EventOutput"), NUM_EVENT_CHANNELS);

        // dummy audio output to ensure the plugin is recognized as an instrument
        // no actual audio will be generated
        addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);
    }
    return result;
}

tresult PLUGIN_API VelocityProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts){
    if(numOuts == 1 && outputs[0] == SpeakerArr::kStereo){
        return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
    }
    return kResultFalse;
}

tresult PLUGIN_API VelocityProcessor::process(ProcessData& data){
    processParameter(data);
    processEvent(data.inputEvents, data.outputEvents);
    return kResultTrue;
}

void VelocityProcessor::processParameter(ProcessData& data){
    if(data.inputParameterChanges == NULL){
        return;
    }

    int8 paramChangeCount = data.inputParameterChanges->getParameterCount();
    int32 sampleOffset;
    ParamValue value;

    for(int8 i = 0; i < data.inputParameterChanges->getParameterCount(); i++){
        IParamValueQueue* queue = data.inputParameterChanges->getParameterData(i);
        if(queue == nullptr){
            continue;
        }
        if(queue->getPoint(queue->getPointCount()-1, sampleOffset, value) == kResultFalse){
            continue;
        }
        switch(queue->getParameterId()){
            case PARAM_ID_VELOCITY:
                this->velocity = value;
                break;
            default:
                // do nothing
                break;
        }
    }
}

void VelocityProcessor::processEvent(IEventList* inputEvents, IEventList* outputEvents){
    if(inputEvents == nullptr){
        return;
    }
    if(outputEvents == nullptr){
        // TODO: warning
        return;
    }

    int32 numEvent = inputEvents->getEventCount();
    Event event;
    for(int32 i = 0; i < numEvent; i++){
        if(inputEvents->getEvent(i, event) == kResultFalse){
            continue;
        }
        switch(event.type){
            case Event::kNoteOnEvent:
                applyVelocityFix(&(event.noteOn));
                break;
            case Event::kNoteOffEvent:
                // do nothing; impl if needed
                break;
            default:
                // do nothing
                break;
        }
        outputEvents->addEvent(event);
    }
}

void VelocityProcessor::applyVelocityFix(NoteOnEvent* noteOnEvent){
    if(noteOnEvent->velocity == 0){  // note off with velocity 0
        return;  // do nothing
    }
    noteOnEvent->velocity = this->velocity;
}


} }