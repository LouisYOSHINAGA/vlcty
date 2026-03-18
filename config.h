#pragma once

#include "pluginterfaces/base/funknown.h"
#include "processor.h"
#include "controller.h"


#define MYVST_VENDOR "Nareshika Works"
#define MYVST_URL "https://github.com/LouisYOSHINAGA/vlcty"
#define MYVST_EMAIL "mailto:9dutat.e@gmail.com"
#define MYVST_VSTNAME "VlctyCrrctr"
#define MYVST_VERSION  "0"
#define MYVST_SUBCATEGORIES Vst::PlugType::kInstrumentSynth


namespace Steinberg{
namespace Vst {


static const FUID ProcessorUID(0x697D656A, 0x4A034B8A, 0x9D03BB47, 0x2952F420);
static const FUID ControllerUID(0x367D1FC0, 0xCC58408D, 0xAE949CC0, 0x37EFB4C2);


} }