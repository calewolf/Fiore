#include "SynthVoice.h"

SynthVoice::SynthVoice() {
    masterGain.setGainLinear (0.7f);
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;
    
    osc1.prepare(spec);
    osc2.prepare(spec);
    masterGain.prepare(spec);
    adsr.setSampleRate(sampleRate);
    
    isPrepared = true;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) {
    std::cout << "Note on! Velocity: " << velocity << std::endl;
    
    auto freqHz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    osc1.setFrequency(freqHz, true);
    osc1.setLevel(velocity * osc1MixRatio);
    osc2.setFrequency(freqHz, true);
    osc2.setLevel(velocity * (1.0 - osc1MixRatio));
    
    adsr.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff) {
    std::cout << "Note off!" << std::endl;
    adsr.noteOff();

    if (!allowTailOff || !adsr.isActive()) {
        clearCurrentNote();
    }
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue) {
    
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue) {
    
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    jassert(isPrepared);
    if (!isVoiceActive()) {
        return;
    }
    
    // 0. Clear and resize a temp buffer to put our processed signal into
    osc1Buffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    osc1Buffer.clear();
    osc2Buffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    osc2Buffer.clear();
    
    juce::dsp::AudioBlock<float> osc1AudioBlock {osc1Buffer};
    juce::dsp::AudioBlock<float> osc2AudioBlock {osc2Buffer};
    
    // 1. Get sounds from the oscillators and add them
    osc1.process(juce::dsp::ProcessContextReplacing<float> (osc1AudioBlock));
    osc2.process(juce::dsp::ProcessContextReplacing<float> (osc2AudioBlock));
    osc2AudioBlock += osc1AudioBlock;cd 
    
    // 2. Apply master gain
    masterGain.process(juce::dsp::ProcessContextReplacing<float> (osc2AudioBlock));
    
    // 2. Apply amplitude ADSR
    adsr.applyEnvelopeToBuffer(osc2Buffer, 0, osc2Buffer.getNumSamples());
    
    // 3. Add the current channel's audio data to the larger outputBuffer
    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++) {
        outputBuffer.addFrom(channel, startSample, osc2Buffer, channel, 0, numSamples);
    }
    
    if (!adsr.isActive()) {
        clearCurrentNote();
    }
}



void SynthVoice::updateADSR(const float attack, const float decay, const float sustain, const float release) {
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;
    adsr.setParameters(adsrParams);
}

void SynthVoice::updateGain (const float gainDecibels) {
//    globalGain.setGainDecibels(gainDecibels);
}

void SynthVoice::setOscWaveform(const int waveformId, const int oscNum) {
    if (oscNum == 1) {
        std::cout << "OSC 1 waveform changed to ID " << waveformId << std::endl;
        osc1.setWaveform(waveformId);
    } else {
        std::cout << "OSC 2 waveform changed to ID " << waveformId << std::endl;
        osc2.setWaveform(waveformId);
    }
}

void SynthVoice::setOscGainRatios(const float val) {
    jassert(0 <= val && val <= 1);
    osc1MixRatio = val;
}
