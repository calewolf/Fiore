#include "SynthVoice.h"

SynthVoice::SynthVoice() {
    auto& masterGain = processorChain.get<masterGainIndex>();
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
    
    processorChain.prepare(spec);
    adsr.setSampleRate(sampleRate);
    
    isPrepared = true;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) {
    std::cout << "Note on! Velocity: " << velocity << std::endl;
    
    auto freqHz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    processorChain.get<osc1Index>().setFrequency(freqHz, true);
    processorChain.get<osc1Index>().setLevel(velocity);
    
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
    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();
    juce::dsp::AudioBlock<float> audioBlock {synthBuffer};
    
    // 1. Get sounds from the oscillator processor chain
    juce::dsp::ProcessContextReplacing<float> context (audioBlock);
    processorChain.process(context);
    
    // 2. Apply amplitude ADSR
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());
    
    // 3. Add the current channel's audio data to the larger outputBuffer
    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++) {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
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
//    auto& oscToChange = oscNum == 1 ? osc1 : osc2;
//    auto& oscToChange = processorChain.template get<0>();
//
//    switch (waveformId) {
//        case 0:
//            oscToChange.initialise([] (float x) { return x / juce::MathConstants<float>::pi; }, 128); // saw
//            break;
//        case 1:
//            oscToChange.initialise ([] (float x) { return x < 0.0f ? -1.0f : 1.0f; }, 128); // square
//            break;
//        case 2:
//            if (oscNum == 1) {
//                oscToChange.initialise ([](float x) { return std::sin (x); }, 128); // noise
//            } else {
//                oscToChange.initialise ([](float x) { return std::sin (x); }, 128); // tri
//            }
//            break;
//        default:
//            jassertfalse;
//            break;
//    }
}

void SynthVoice::setOscGainRatios(const float osc1Amount) {
    jassert(osc1Amount >= 0 && osc1Amount <= 1);

//    osc1Gain.setGainLinear(osc1Amount);
//    osc2Gain.setGainLinear(1.0 - osc1Amount);
}
