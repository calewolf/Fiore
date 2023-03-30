#pragma once
#include <JuceHeader.h>

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
    
    private:
        juce::ADSR adsr;
        juce::ADSR::Parameters adsrParams;
        juce::AudioBuffer<float> synthBuffer;
    
        juce::dsp::Oscillator<float> osc;
        juce::dsp::Gain<float> gain;
        bool isPrepared {false};
};
