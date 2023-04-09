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
#include "FxModule.h"
#include "BendModule.h"
#include "LFOVibratoModule.h"
#include "FilterEnvModule.h"
#include "AmpEnvModule.h"


class Editor: public AudioProcessorEditor {
    
public:
    Editor(CapstoneSynthAudioProcessor&);
    ~Editor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OscillatorModule oscModule;
    FilterModule filterModule;
    AmpModule ampModule;
    FxModule fxModule;
    BendModule bendModule;
    LFOVibratoModule lfoVibratoModule;
    FilterEnvModule filterEnvModule;
    AmpEnvModule ampEnvModule;
    
    CapstoneSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
