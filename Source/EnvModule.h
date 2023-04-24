#pragma once

#include <JuceHeader.h>

/**
    A single tab of the EnvModule. Is reused for both amplitude and filter envelopes.
*/
class EnvTab: public juce::Component {
    public:
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        EnvTab(const String& title, juce::AudioProcessorValueTreeState& apvts, const String& paramIdPrefix);
        ~EnvTab();
        void paint (juce::Graphics&) override;
        void resized() override;
    
    private:
        void configureLabel(juce::Component& slider, juce::Label& label, const juce::String& labelText);
        void configureSlider(juce::Slider& slider, const juce::String textValueSuffix, int numDecimalPlacesToDisplay, const juce::String& paramID, std::unique_ptr<SliderAttachment>& attachmentToCreate);
    
        juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
        juce::Label attackSliderLabel, decaySliderLabel, sustainSliderLabel, releaseSliderLabel;
        juce::Label titleLabel;
    
        juce::AudioProcessorValueTreeState& apvts;
        std::unique_ptr<SliderAttachment> attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;
};

/**
    A module containing sliders to control an ADSR envelope. Used to control both amplitude and filter cutoff envelopes.
*/
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
