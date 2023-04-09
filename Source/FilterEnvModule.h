/*
  ==============================================================================

    FilterEnvModule.h
    Created: 9 Apr 2023 1:40:47pm
    Author:  Cale Wolf

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class FilterEnvModule  : public juce::Component
{
public:
    FilterEnvModule();
    ~FilterEnvModule() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label filterEnvLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterEnvModule)
};
