#pragma once

#include "public.sdk/source/vst/vstguieditor.h"
#include "pluginterfaces/vst/ivstplugview.h"
#include "param.h"


// #define DEBUG_GUIEDITOR


namespace Steinberg {
namespace Vst {
using namespace VSTGUI;


class VelocityGUIEditor : public VSTGUIEditor,
                          public IControlListener {
    protected:
        static constexpr uint16 BG_WIDTH = 400;
        static constexpr uint16 BG_HEIGHT = 185;
        void createHorizontalSlider(ParamID, uint16, uint16);
    public:
        VelocityGUIEditor(EditController*);
        virtual bool PLUGIN_API open(void*, const PlatformType& = PlatformType::kDefaultNative);
        virtual void PLUGIN_API close();
        void valueChanged(CControl*);
        void createLabel(UTF8StringPtr, uint16, uint16, uint16, uint16,
                         uint8 = 18, bool = false, CHoriTxtAlign = kLeftText);
        void createCombobox(ParamID, std::vector<UTF8StringPtr>, uint16, uint16, uint16, uint16,
                            uint8 = 18, bool = false, CHoriTxtAlign = kLeftText);
        void createSlider(ParamID, uint16, uint16);
        DELEGATE_REFCOUNT(VSTGUIEditor)
};


} }