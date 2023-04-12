#pragma once

#include <JuceHeader.h>

class EnvTab: public juce::Component {
    public:
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        EnvTab(const String& title, juce::AudioProcessorValueTreeState& apvts);
        ~EnvTab();
        void paint (juce::Graphics&) override;
        void resized() override;
    
    private:
        void configureLabel(juce::Component& slider, juce::Label& label, const juce::String& labelText);
        void configureSlider(juce::Slider& slider, const juce::String textValueSuffix, int numDecimalPlacesToDisplay, const juce::String& paramID, std::unique_ptr<SliderAttachment>& attachmentToCreate);
    
        juce::AudioProcessorValueTreeState& apvts;
    
        juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
        juce::Label attackSliderLabel, decaySliderLabel, sustainSliderLabel, releaseSliderLabel;
        juce::Label titleLabel;
    
        std::unique_ptr<SliderAttachment> attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;
};

class EnvModule: public juce::Component {
    public:
        EnvModule(juce::AudioProcessorValueTreeState& apvts);
        ~EnvModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::TabbedComponent tabs {TabbedButtonBar::Orientation::TabsAtTop};
        EnvTab ampEnvTab, filterEnvTab;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvModule)
};
