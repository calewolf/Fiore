#pragma once
#include <JuceHeader.h>
#include "CustomOsc.h"

/**
 This class is kind of useless but is required by the Synthesiser class.
*/
class SynthSound: public juce::SynthesiserSound {
    public:
        SynthSound() {}
        bool appliesToNote (int) override { return true; }
        bool appliesToChannel (int) override { return true; }
};

/**
    Represents a single voice being played in the synth. Contains all the DSP / audio processing code to render sounds.
*/
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
        
        // Sets up voice for playback
        void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    
        // Functions to change synth parameters
        void setOscWaveform(int waveformId, int oscNum);
        void setOscGainRatios(float osc1Amount);
        void setOscDetune(int semitones, int cents);
        void setOscSineLevel(float percent);
        void setFilterType(int filterTypeIdx);
        void setFilterParams(float cutoffHz, float resonance, float driveAmt, float envAmt);
        void setFilterOnOff(bool filterShouldBeOn);
        void setMasterGain(float decibels);
        void setFilterLFOParams(int lfoShapeId, float ampPercent, float rateHz);
        void setVibratoParams(int lfoShapeId, float ampPercent, float rateHz);
        void setAmpADSR(float attack, float decay, float sustain, float release);
        void setFilterADSR(float attack, float decay, float sustain, float release);
        
    private:
        juce::AudioBuffer<float> osc1Buffer, osc2Buffer;
    
        // DSP Components:
        CustomOscillator<float> osc1, osc2;
        juce::dsp::Gain<float> masterGain;
        juce::dsp::LadderFilter<float> filter;
        juce::ADSR adsr;
        juce::ADSR filterAdsr;
        juce::dsp::Oscillator<float> lfo;
        juce::dsp::Oscillator<float> vibratoLfo;
    
        /// The mix between OSC1 and OSC2.
        float osc1MixRatio { 0.5 };
        /// The velocity from the last time `startNote` was called.
        float currentVelocity;
        /// The amount of semitones to detune Oscillator 2 by.
        float detuneSemitones;
        /// The frequency from the last time `startNote` was called.
        float baseFreqHz;
        /// An error flag to indicate that `prepareToPlay` finished, so that `renderNextBlock` doesn't call prematurely.
        bool isPrepared {false};
        /// Whether the filter is on or off
        bool filterIsOn;
    
        float baseCutoffHz;
    
        /// How much the LFO actually modifies the filter's cutoff. Ranges from 0-1.
        float lfoCutoffDepth;
    
        float vibratoDepth;
    
        /// How much the filter envelope modifies the cutoff. Ranges from 0-1.
        float filterEnvDepth;
};
