#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "OscillatorModule.h"
#include "FilterModule.h"
#include "AmpModule.h"
#include "LFOVibratoModule.h"
#include "EnvModule.h"

/**
    The main entrypoint for the synth's GUI. Contains subcomponents for each module of the synth.
*/
class Editor: public AudioProcessorEditor {
    public:
        Editor(FioreAudioProcessor&);
        ~Editor() override;
        void paint(juce::Graphics&) override;
        void resized() override;

    private:
        FioreAudioProcessor& audioProcessor;
        
        OscillatorModule oscModule;
        FilterModule filterModule;
        LFOVibratoModule lfoVibratoModule;
        EnvModule envModule;
        AmpModule ampModule;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
