#pragma once

#include <JuceHeader.h>

class EnvTab: public juce::Component {
    public:
        EnvTab(const String& title);
        ~EnvTab();
        void paint (juce::Graphics&) override;
        void resized() override;
    private:
        juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
        juce::Label attackSliderLabel, decaySliderLabel, sustainSliderLabel, releaseSliderLabel;
        juce::Label titleLabel;
};

class EnvModule: public juce::Component {
    public:
        EnvModule();
        ~EnvModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::TabbedComponent tabs {TabbedButtonBar::Orientation::TabsAtTop};
        EnvTab ampEnvTab {"AMP ENV"}, filterEnvTab {"FILTER ENV"};
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvModule)
};
