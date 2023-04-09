#pragma once

#include <JuceHeader.h>

class AmpModule: public juce::Component {
    public:
        AmpModule();
        ~AmpModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::Label volumeSliderLabel, sineLevelSliderLabel;
        juce::Slider volumeSlider, sineLevelSlider;
        
        juce::Label ampModuleLabel;
    
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModule)
};
