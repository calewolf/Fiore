#pragma once
#include <JuceHeader.h>
#include "CustomOsc.h"

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
        
        // Sets up voice for playback
        void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    
        // Functions to change synth parameters
        void setOscWaveform(int waveformId, int oscNum);
        void setOscGainRatios(float osc1Amount);
        void setOscDetune(float semitones, float cents);
        void setOscVibratoDepth(float semitones);
        void setOscSineLevel(float percent);
        void setFilterType(int filterTypeId);
        void setFilterParams(float cutoffHz, float resonance, float lfoAmt, float envAmt);
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
        float osc1MixRatio { 0.5 };
    
        juce::ADSR adsr;
        juce::ADSR::Parameters adsrParams;
    
        bool isPrepared {false};
};
