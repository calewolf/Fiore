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
        void setOscDetune(int semitones, int cents, int oscNum);
        void setOscSineLevel(float percent);
        void setFilterType(int filterTypeIdx);
        void setFilterParams(float cutoffHz, float resonance, float driveAmt, float envAmt);
        void setFilterOnOff(bool filterShouldBeOn);
        void setMasterGain(float decibels);
        void setLFOParams(int lfoShapeId, float ampPercent, float rateHz, int lfoIdToModify);
        void setAmpADSR(float attack, float decay, float sustain, float release);
        void setFilterADSR(float attack, float decay, float sustain, float release);
        
    private:
        /// Calculates the adjusted `baseFreqHz` after vibrato is applied.
        float getNewFreqFromVibratoLFO(int numSamples);
    
        /// Calculates the adjusted `baseCutoffHz` after the filter LFO and envelope are applied.
        float getCutoffFromEnvAndLFO(int numSamples);
    
        // Temp buffers to load processed signals into
        juce::AudioBuffer<float> osc1Buffer, osc2Buffer, sineBuffer;
    
        // Things that modify/process the oscillator audio:
        CustomOscillator<float> osc1, osc2, sineOsc;
        juce::dsp::Gain<float> masterGain;
        juce::dsp::LadderFilter<float> filter;
        juce::ADSR adsr;
        juce::ADSR filterAdsr;
        juce::dsp::Oscillator<float> lfo;
        juce::dsp::Oscillator<float> vibratoLfo;
    
        // Helper Variables
        /// The mix between OSC1 and OSC2. 1.0 = 100% OSC1.
        float osc1MixRatio { 0.5 };
        /// The amount of sine to mix into the signal.
        float sineLevel;
        /// The velocity from the last time `startNote` was called.
        float currentVelocity;
        /// The frequency from the last time `startNote` was called.
        float baseFreqHz;
        /// The number of semitones the pitch wheel has set the synth to detune by
        float pitchWheelDetuneSemitones { 0.0 };
        /// The amount of semitones to detune Oscillator 2 by.
        float osc2DetuneSemitones;
        /// The amount of semitones to detune Oscillator 1 by.
        float osc1DetuneSemitones;
        /// Whether the filter is on or off
        bool filterIsOn;
        /// The "center" cutoff frequency from the filter.
        float baseCutoffHz;
        /// How much the LFO actually modifies the filter's cutoff. Ranges from 0-1.
        float lfoCutoffDepth;
        /// How much the vibrato LFO modifies the pitch. Ranges from 0-1.
        float vibratoDepth;
        /// How much the filter envelope modifies the cutoff. Ranges from 0-1.
        float filterEnvDepth;
        /// An error flag to indicate that `prepareToPlay` finished, so that `renderNextBlock` doesn't get called prematurely.
        bool isPrepared {false};
        /// A Random object for use in creating white noise
        Random random;
};
