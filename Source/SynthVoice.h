#pragma once
#include <JuceHeader.h>
#include "CustomOsc.h"

// TODO: What's the importance of this class?
class SynthSound: public juce::SynthesiserSound {
    public:
        SynthSound() {}
        bool appliesToNote (int) override { return true; }
        bool appliesToChannel (int) override { return true; }
};

class SynthVoice: public juce::SynthesiserVoice {
    public:
        // SynthesiserVoice overrides
        SynthVoice();
        bool canPlaySound (juce::SynthesiserSound* sound) override;
        void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
        void stopNote (float velocity, bool allowTailOff) override;
        void controllerMoved (int controllerNumber, int newControllerValue) override;
        void pitchWheelMoved (int newPitchWheelValue) override;
        void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
        
        void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    
        /// Updates the value of `adsrParams` to change the amplitude's ADSR. Called from `pluginProcessor`.
        void updateADSR (const float attack, const float decay, const float sustain, const float release);
        void updateGain (const float gainDecibels);
        void setOscWaveform(const int waveformId, const int oscNum);
        void setOscGainRatios(const float osc1Amount);
    
    private:
        juce::AudioBuffer<float> synthBuffer;
    
        enum {
            osc1Index,
            masterGainIndex
        };
    
        /// Oscillator 1: Contains a gain for its relative gain w/ osc 2
        juce::dsp::ProcessorChain<CustomOscillator<float>, juce::dsp::Gain<float>> processorChain;
    
        juce::ADSR adsr;
        juce::ADSR::Parameters adsrParams;
    
        bool isPrepared {false};
};
