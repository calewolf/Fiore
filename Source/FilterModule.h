#pragma once

#include <JuceHeader.h>

class FilterModule: public juce::Component, public juce::Button::Listener {
    public:
        FilterModule();
        ~FilterModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;
        void buttonClicked (juce::Button* button) override;

    private:
        juce::TextButton onOffButton {"On"};
        juce::ComboBox filterTypeMenu;
    
        juce::Label cutoffSliderLabel, resonanceSliderLabel;
        juce::Slider cutoffSlider, resonanceSlider;
        
        juce::Label lfoSliderLabel, envSliderLabel;
        juce::Slider lfoSlider, envSlider;
        
        juce::Label filterModuleLabel;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterModule)
};
