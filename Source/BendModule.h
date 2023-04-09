#pragma once

#include <JuceHeader.h>


class BendModule: public juce::Component {
public:
    BendModule();
    ~BendModule() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Label bendModuleLabel;
    juce::Label timeSliderLabel, depthSliderLabel;
    juce::Slider timeSlider, depthSlider;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BendModule)
};
