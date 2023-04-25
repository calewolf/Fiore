#include "SynthVoice.h"

SynthVoice::SynthVoice() {}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;
    
    // Required to call `prepare` on each of the juce::dsp processors.
    osc1.prepare(spec);
    osc2.prepare(spec);
    masterGain.prepare(spec);
    filter.prepare(spec);
    lfo.prepare(spec);
    vibratoLfo.prepare(spec);
    
    adsr.setSampleRate(sampleRate);
    filterAdsr.setSampleRate(sampleRate);
    isPrepared = true;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) {
    baseFreqHz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    osc1.setLevel(velocity * osc1MixRatio);
    osc2.setLevel(velocity * (1.0 - osc1MixRatio));
    adsr.noteOn();
    filterAdsr.noteOn();
    currentVelocity = velocity;
}

void SynthVoice::stopNote(float velocity, bool allowTailOff) {
    adsr.noteOff();
    filterAdsr.noteOff();
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
    osc2Buffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    osc1Buffer.clear();
    osc2Buffer.clear();
    juce::dsp::AudioBlock<float> osc1AudioBlock {osc1Buffer};
    juce::dsp::AudioBlock<float> osc2AudioBlock {osc2Buffer};
    
   // 1. Adjust the oscillators' frequencies with vibrato
    float adjustedBaseFreq = getNewFreqFromVibratoLFO(numSamples);
    osc1.setFrequency(adjustedBaseFreq, true);
    osc2.setFrequency(adjustedBaseFreq * pow(2, detuneSemitones / 12.0), true);
    
    // 1. Get sounds from the oscillators and add them
    osc1.process(juce::dsp::ProcessContextReplacing<float> (osc1AudioBlock));
    osc2.process(juce::dsp::ProcessContextReplacing<float> (osc2AudioBlock));
    osc2AudioBlock += osc1AudioBlock;
    
    // 2. Apply filter (modulated with envelope, filter LFO)
    if (filterIsOn) {
        filter.setCutoffFrequencyHz(getCutoffFromEnvAndLFO(numSamples));
        filter.process(juce::dsp::ProcessContextReplacing<float>(osc2AudioBlock));
    }
    
    // 3. Apply amplitude ADSR
    adsr.applyEnvelopeToBuffer(osc2Buffer, 0, osc2Buffer.getNumSamples());
    
    // 4. Apply master gain
    masterGain.process(juce::dsp::ProcessContextReplacing<float>(osc2AudioBlock));
    
    // 5. Add the current channel's audio data to the larger outputBuffer
    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++) {
        outputBuffer.addFrom(channel, startSample, osc2Buffer, channel, 0, numSamples);
    }
    
    if (!adsr.isActive()) {
        clearCurrentNote();
    }
}

float SynthVoice::getNewFreqFromVibratoLFO(int numSamples) {
    float vibratoLfoOut = 0;
    for (int s = 0; s < numSamples; s++) {
        vibratoLfoOut += vibratoLfo.processSample(0.0f);
    }
    vibratoLfoOut /= ((float) numSamples); // -1 to 1

    float multiplier = pow(juce::jmap(vibratoDepth, 0.0f, 1.0f, 1.0f, 1.15f), vibratoLfoOut);
    return juce::jmin(juce::jmax(baseFreqHz * multiplier, 20.0f), 20000.0f);
}

float SynthVoice::getCutoffFromEnvAndLFO(int numSamples) {
    float lfoOut = 0;
    float avgFiltAdsrOut = 0;
    for (int s = 0; s < numSamples; s++) {
        lfoOut += lfo.processSample(0.0f);
        avgFiltAdsrOut += filterAdsr.getNextSample();
    }
    lfoOut /= ((float) numSamples);
    avgFiltAdsrOut /= ((float) numSamples);
    
    // 2.1 Modify the base cutoff according to the current env value and maximum env depth
    auto baseCutoffMultiplier = juce::jmap(avgFiltAdsrOut, 0.0f, 1.0f, 1.0f - filterEnvDepth, 1.0f);
    
    // 2.2 Add/subtract from the base cutoff according to the LFO value
    // FIXME: This is mapping from 0 to 1000 Hz. This needs to exist on an exponential scale.
    auto cutoffMod = juce::jmap(lfoCutoffDepth, 0.0f, 1.0f, 0.0f, 1000.0f) * lfoOut;
    auto newCutoff = (baseCutoffHz * baseCutoffMultiplier) + cutoffMod;
    return juce::jmin(juce::jmax(newCutoff, 20.0f), 20000.0f);
}

// OSC Module Setters

void SynthVoice::setOscWaveform(int waveformId, int oscNum) {
    if (oscNum == 1) {
        osc1.setWaveform(waveformId);
    } else {
        osc2.setWaveform(waveformId);
    }
}

void SynthVoice::setOscGainRatios(float val) {
    jassert(0 <= val && val <= 1);
    osc1MixRatio = val;
    osc1.setLevel(currentVelocity * osc1MixRatio);
    osc2.setLevel(currentVelocity * (1.0 - osc1MixRatio));
}

void SynthVoice::setOscDetune(int semitones, int cents) {
    detuneSemitones = semitones + ((float) cents / 100.0);
//    osc2.setFrequency(baseFreqHz * pow(2, detuneSemitones / 12.0), true);
}

void SynthVoice::setOscSineLevel(float percent) {
    // TODO: Implement
    return;
}

// Filter Module Setters

void SynthVoice::setFilterType(int filterTypeIdx) {
    dsp::LadderFilterMode modeToAssign;
    switch (filterTypeIdx) {
        case 0:
            modeToAssign = dsp::LadderFilterMode::LPF12;
            break;
        case 1:
            modeToAssign = dsp::LadderFilterMode::HPF12;
            break;
        case 2:
            modeToAssign = dsp::LadderFilterMode::BPF12;
            break;
        case 3:
            modeToAssign = dsp::LadderFilterMode::LPF24;
            break;
        case 4:
            modeToAssign = dsp::LadderFilterMode::HPF24;
            break;
        case 5:
            modeToAssign = dsp::LadderFilterMode::BPF24;
            break;
        default:
            jassertfalse;
            break;
    }
    filter.setMode(modeToAssign);
}

void SynthVoice::setFilterParams(float cutoffHz, float resonance, float driveAmt, float envAmt) {
    baseCutoffHz = cutoffHz;
    filter.setResonance(resonance);
    filter.setDrive(juce::jmap(driveAmt, 0.0f, 1.0f, 1.0f, 10.0f));
    filterEnvDepth = envAmt;
}

void SynthVoice::setFilterOnOff(bool filterShouldBeOn) {
    filterIsOn = filterShouldBeOn;
}

// Amp Module Setters

void SynthVoice::setMasterGain(float gainDecibels) {
    masterGain.setGainDecibels(gainDecibels);
}

// Vibrato / LFO Module Setters

void SynthVoice::setFilterLFOParams(int lfoShapeId, float ampPercent, float rateHz) {
    switch (lfoShapeId) {
        case 0: // Saw up
            lfo.initialise([] (float x) { return juce::jmap (x, -juce::MathConstants<float>::pi, juce::MathConstants<float>::pi, -1.0f, 1.0f); }, 2);
            break;
        case 1: // Saw down
            lfo.initialise([] (float x) { return juce::jmap (x, -juce::MathConstants<float>::pi, juce::MathConstants<float>::pi, 1.0f, -1.0f); }, 2);
            break;
        case 2: // Tri
            lfo.initialise([](float x) { // triangle
                if (x < 0.0f) {
                    return juce::jmap (x, -juce::MathConstants<float>::pi, 0.0f, -1.0f, 1.0f);
                } else {
                    return juce::jmap (x, 0.0f, juce::MathConstants<float>::pi, 1.0f, -1.0f);
                }
            }, 3);
            break;
        case 3: // Square
            lfo.initialise ([] (float x) { return x < 0.0f ? -1.0f : 1.0f; }, 128);
            break;
            // TODO: Add a noise option
        default:
            jassertfalse;
            break;
    }
    
    lfoCutoffDepth = ampPercent; // 0.0 to 1.0
    lfo.setFrequency(rateHz);
}

void SynthVoice::setVibratoParams(int lfoShapeId, float ampPercent, float rateHz) {
    switch (lfoShapeId) {
        case 0: // Saw up
            vibratoLfo.initialise([] (float x) { return juce::jmap (x, -juce::MathConstants<float>::pi, juce::MathConstants<float>::pi, -1.0f, 1.0f); }, 2);
            break;
        case 1: // Saw down
            vibratoLfo.initialise([] (float x) { return juce::jmap (x, -juce::MathConstants<float>::pi, juce::MathConstants<float>::pi, 1.0f, -1.0f); }, 2);
            break;
        case 2: // Tri
            vibratoLfo.initialise([](float x) { // triangle
                if (x < 0.0f) {
                    return juce::jmap (x, -juce::MathConstants<float>::pi, 0.0f, -1.0f, 1.0f);
                } else {
                    return juce::jmap (x, 0.0f, juce::MathConstants<float>::pi, 1.0f, -1.0f);
                }
            }, 3);
            break;
        case 3: // Square
            vibratoLfo.initialise ([] (float x) { return x < 0.0f ? -1.0f : 1.0f; }, 128);
            break;
        // TODO: Add a noise option
        default:
            jassertfalse;
            break;
    }
    
    vibratoDepth = ampPercent; // 0.0 to 1.0
    vibratoLfo.setFrequency(rateHz);
}

// Envelope Module Setters

void SynthVoice::setAmpADSR(float attack, float decay, float sustain, float release) {
    adsr.setParameters(juce::ADSR::Parameters {attack, decay, sustain, release});
}

void SynthVoice::setFilterADSR(float attack, float decay, float sustain, float release) {
    filterAdsr.setParameters(juce::ADSR::Parameters {attack, decay, sustain, release});
}
