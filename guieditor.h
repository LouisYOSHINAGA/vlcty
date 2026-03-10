#pragma once

#include "public.sdk/source/vst/vstguieditor.h"
#include "pluginterfaces/vst/ivstplugview.h"
#include "config.h"


namespace Steinberg {
namespace Vst {
using namespace VSTGUI;


class VelocityGUIEditor : public VSTGUIEditor,
                          public IControlListener {
    protected:
        static constexpr uint16 BG_WIDTH = 400;
        static constexpr uint16 BG_HEIGHT = 185;
    public:
        VelocityGUIEditor(EditController*);
        virtual bool PLUGIN_API open(void*, const PlatformType&);
        virtual void PLUGIN_API close();
        void valueChanged(CControl*);
        void createSlider(ParamID, uint16, uint16);

        DELEGATE_REFCOUNT(VSTGUIEditor)
};


} }