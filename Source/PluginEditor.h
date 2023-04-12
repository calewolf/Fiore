/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "OscillatorModule.h"
#include "FilterModule.h"
#include "AmpModule.h"
#include "LFOVibratoModule.h"
#include "EnvModule.h"

class Editor: public AudioProcessorEditor {
    
public:
    Editor(CapstoneSynthAudioProcessor&);
    ~Editor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    CapstoneSynthAudioProcessor& audioProcessor;
    
    OscillatorModule oscModule;
    FilterModule filterModule;
    AmpModule ampModule;
    LFOVibratoModule lfoVibratoModule;
    EnvModule envModule { audioProcessor.getAPVTS() };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
