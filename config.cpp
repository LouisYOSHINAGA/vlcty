#include "config.h"
#include "public.sdk/source/main/pluginfactory.h"


extern "C"{
    BEGIN_FACTORY_DEF(MYVST_VENDOR, MYVST_URL, MYVST_EMAIL)
        // VelocityProcessor
        DEF_CLASS2(
            INLINE_UID_FROM_FUID(Steinberg::Vst::ProcessorUID),
            PClassInfo::kManyInstances,
            kVstAudioEffectClass,
            MYVST_VSTNAME,
            Vst::kDistributable,
            MYVST_SUBCATEGORIES,
            MYVST_VERSION,
            kVstVersionString,
            Steinberg::Vst::VelocityProcessor::createInstance
        )
        // VelocityController
        DEF_CLASS2(
            INLINE_UID_FROM_FUID(Steinberg::Vst::ControllerUID),
            PClassInfo::kManyInstances,
            kVstComponentControllerClass,
            MYVST_VSTNAME " Controller",
            0,
            "",
            MYVST_VERSION,
            kVstVersionString,
            Steinberg::Vst::VelocityController::createInstance
        )
    END_FACTORY
}