#pragma once

#include <JuceHeader.h>

class AmpModule: public juce::Component {
    public:
        AmpModule();
        ~AmpModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::Label volumeSliderLabel;
        juce::Slider volumeSlider;
        
        juce::Label ampModuleLabel;
    
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModule)
};
