#pragma once
#include <JuceHeader.h>

class FxModule: public juce::Component {
    public:
        FxModule();
        ~FxModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::TextButton onOffButton {"On"};
        juce::ComboBox effectChoiceMenu;
        
        juce::Label mixSliderLabel, rateSliderLabel;
        juce::Slider mixSlider, rateSlider;
        
        juce::Label fxModuleLabel;
    
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxModule)
};
