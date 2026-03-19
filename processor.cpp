#include "processor.h"
#include "config.h"
#include <algorithm>


namespace Steinberg {
namespace Vst {


FUnknown* VelocityProcessor::createInstance(void*){
    return (IAudioProcessor*)new VelocityProcessor();
}

VelocityProcessor::VelocityProcessor()
    : correctType(CORRECT_TYPE_THROUGH)
    , fixVelocity(static_cast<float>(DEFAULT_VELOCITY) / N_VELOCITY_STEPS)
    , minVelocity(static_cast<float>(DEFAULT_VELOCITY) / N_VELOCITY_STEPS)
    , maxVelocity(static_cast<float>(DEFAULT_VELOCITY) / N_VELOCITY_STEPS)
    , inputVelocity(DEFAULT_VELOCITY)
    , outputVelocity(DEFAULT_VELOCITY){
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

tresult PLUGIN_API VelocityProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns,
                                                         SpeakerArrangement* outputs, int32 numOuts){
    if(numOuts == 1 && outputs[0] == SpeakerArr::kStereo){
        return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
    }
    return kResultFalse;
}

tresult PLUGIN_API VelocityProcessor::process(ProcessData& data){
    processParameter(data);
    processEvent(data);
    return kResultTrue;
}

void VelocityProcessor::processParameter(ProcessData& data){
    if(data.inputParameterChanges == NULL){
        return;
    }

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
            case PARAM_ID_CORRECT_TYPE:
                this->correctType = static_cast<CorrectTypeID>(
                    static_cast<int8>(value * (N_CORRECT_TYPES - 1) + EPSILON)
                );
                break;
            case PARAM_ID_VELOCITY_FIX:
                this->fixVelocity = value;
                break;
            case PARAM_ID_VELOCITY_MIN:
                this->minVelocity = value;
                break;
            case PARAM_ID_VELOCITY_MAX:
                this->maxVelocity = value;
                break;
            default:
                // do nothing
                break;
        }
    }
}

void VelocityProcessor::processEvent(ProcessData& data){
    IEventList* inputEvents = data.inputEvents;
    IEventList* outputEvents = data.outputEvents;

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
                applyVelocityFix(event.noteOn);

                if(data.outputParameterChanges){
                    this->sendParameterToController(PARAM_ID_INPUT_VELOCITY, this->inputVelocity,
                                                    *data.outputParameterChanges, event.sampleOffset);
                    this->sendParameterToController(PARAM_ID_OUTPUT_VELOCITY, this->outputVelocity,
                                                    *data.outputParameterChanges, event.sampleOffset);
                }
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

void VelocityProcessor::applyVelocityFix(NoteOnEvent& noteOnEvent){
    if(noteOnEvent.velocity == 0){  // note off with velocity 0
        return;  // do nothing
    }

    this->inputVelocity = noteOnEvent.velocity;  // store input velocity

    switch(this->correctType){
        case CORRECT_TYPE_THROUGH:
            // do nothing
            break;
        case CORRECT_TYPE_FIX:
            noteOnEvent.velocity = this->fixVelocity;
            break;
        case CORRECT_TYPE_REMAP:{
            float v = (noteOnEvent.velocity - DEFACTO_MIN_VELOCITY) / (1.0 - DEFACTO_MIN_VELOCITY);
            v = std::clamp(v, 0.0f, 1.0f);
            noteOnEvent.velocity = this->minVelocity + v * (this->maxVelocity - this->minVelocity);
            break;
        }
        case CORRECT_TYPE_CLIP:
            if(this->minVelocity > this->maxVelocity){  // invalid setting
                noteOnEvent.velocity = 0;
            }else if(noteOnEvent.velocity < this->minVelocity){
                noteOnEvent.velocity = this->minVelocity;
            }else if(this->maxVelocity < noteOnEvent.velocity){
                noteOnEvent.velocity = this->maxVelocity;
            }
            break;
        default:
            // do nothing
            break;
    }

    this->outputVelocity = noteOnEvent.velocity;  // store output velocity
}

void VelocityProcessor::sendParameterToController(ParamID paramId, float rawVelocity,
                                                  IParameterChanges& outParamChanges, int32 sampleOffset){
    int32 queueIndex = 0;
    IParamValueQueue* queue = outParamChanges.addParameterData(paramId, queueIndex);  // plugin -> DAW
    if(queue){
        queue->addPoint(sampleOffset, rawVelocity, queueIndex);
    }
}


} }