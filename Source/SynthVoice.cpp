#include "SynthVoice.h"

SynthVoice::SynthVoice() {
    osc.initialise ([] (float x) { return std::sin (x); }, 128);
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) {
    osc.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    gain.setGainLinear(velocity * 0.15);
    adsr.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff) {
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
    
    /*
     IMPORTANT NOTE:
     
     Since notes may start on *any* sample, the parent Synthesiser class may call a voice's renderNextBlock
     in the middle of an output buffer. It doesn't necessarily align with the start of each output buffer in
     PluginProcessor's processBlock(). So startSample really matters here (it's not always 0).
     
     We also need to overlap-add the result of this funciton to outputBuffer because of this issue, and the fact
     that renderNextBlock is being called for each voice, and the input from outputBuffer can contain outputs from
     other voices.
     */
    
    // If the voice is not playing any sounds, we don't need to modify the buffer at all.
    if (!isVoiceActive()) {
        return;
    }
    
    // Clear and resize a temp buffer to put our processed signal into
    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();
    
    // Apply signal processing to our buffer
    // This is a shortcut around writing everything myself (i.e. oscillators, adsr envelopes, etc.)
    juce::dsp::AudioBlock<float> audioBlock {synthBuffer};
    osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());
    
    // Add the current channel's audio data to the larger outputBuffer
    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++) {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
    }
    
    // When the ADSR finishes its tail-off, we need to call this to reset the state of the note.
    if (!adsr.isActive()) {
        clearCurrentNote();
    }
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;
    
    adsr.setSampleRate(sampleRate);
    osc.prepare(spec);
    gain.prepare(spec);
    
    isPrepared = true;
}

void SynthVoice::updateADSR(const float attack, const float decay, const float sustain, const float release) {
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;
    adsr.setParameters(adsrParams);
}
