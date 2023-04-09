/*
  ==============================================================================

    AmpEnvModule.h
    Created: 9 Apr 2023 1:40:55pm
    Author:  Cale Wolf

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class AmpEnvModule  : public juce::Component
{
public:
    AmpEnvModule();
    ~AmpEnvModule() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label ampEnvLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpEnvModule)
};
