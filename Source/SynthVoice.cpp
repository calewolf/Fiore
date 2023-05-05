#include "SynthVoice.h"

SynthVoice::SynthVoice() {
}

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
    sineOsc.prepare(spec);
    masterGain.prepare(spec);
    filter.prepare(spec);
    lfo.prepare(spec);
    vibratoLfo.prepare(spec);
    limiter.prepare(spec);
    limiter.setThreshold(-0.1f);
    limiter.setRelease(1000);
    
    sineOsc.setWaveform(4);
    adsr.setSampleRate(sampleRate);
    filterAdsr.setSampleRate(sampleRate);
    isPrepared = true;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) {
    lfo.reset();
    vibratoLfo.reset();
    
    baseFreqHz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    osc1.setLevel(velocity * 0.15 * osc1MixRatio);
    osc2.setLevel(velocity * 0.15 * (1.0 - osc1MixRatio));
    sineOsc.setLevel(velocity * 0.15 * sineLevel);
    adsr.noteOn();
    filterAdsr.noteOn();
    currentVelocity = velocity * 0.15;
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
    pitchWheelDetuneSemitones = juce::jmap((float) newPitchWheelValue, 0.0f, 16383.0f, -2.0f, 2.0f);;
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    jassert(isPrepared);
    if (!isVoiceActive()) {
        return;
    }
    
    // 0. Clear and resize a temp buffer to put our processed signal into
    osc1Buffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    osc2Buffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    sineBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    osc1Buffer.clear();
    osc2Buffer.clear();
    sineBuffer.clear();
    juce::dsp::AudioBlock<float> osc1AudioBlock {osc1Buffer};
    juce::dsp::AudioBlock<float> osc2AudioBlock {osc2Buffer};
    juce::dsp::AudioBlock<float> sineAudioBlock {sineBuffer};
    
   // 1. Adjust the oscillators' frequencies with vibrato, detune, and pitch wheel
    float adjustedBaseFreq = getNewFreqFromVibratoLFO(numSamples);
    osc1.setFrequency(adjustedBaseFreq * pow(2, (osc1DetuneSemitones + pitchWheelDetuneSemitones) / 12.0), true);
    osc2.setFrequency(adjustedBaseFreq * pow(2, (osc2DetuneSemitones + pitchWheelDetuneSemitones) / 12.0), true);
    sineOsc.setFrequency(adjustedBaseFreq * pow(2, pitchWheelDetuneSemitones / 12.0), true);
    
    // 1. Get sounds from the oscillators and add them
    osc1.process(juce::dsp::ProcessContextReplacing<float> (osc1AudioBlock));
    osc2.process(juce::dsp::ProcessContextReplacing<float> (osc2AudioBlock));
    sineOsc.process(juce::dsp::ProcessContextReplacing<float> (sineAudioBlock));
    osc2AudioBlock += osc1AudioBlock;
    osc2AudioBlock += sineAudioBlock;
    
    // 2. Apply filter (modulated with envelope, filter LFO)
    if (filterIsOn) {
        filter.setCutoffFrequencyHz(getCutoffFromEnvAndLFO(numSamples));
        filter.process(juce::dsp::ProcessContextReplacing<float>(osc2AudioBlock));
    }
    
    // 3. Apply amplitude ADSR
    adsr.applyEnvelopeToBuffer(osc2Buffer, 0, osc2Buffer.getNumSamples());
    
    // Limit with a limiter at this point! Otherwise it can get way too loud!
    limiter.process(juce::dsp::ProcessContextReplacing<float>(osc2AudioBlock));
    
    // 4. Apply master gain
    masterGain.process(juce::dsp::ProcessContextReplacing<float>(osc2AudioBlock));
    
    // 5. Add the current channel's audio data to the larger outputBuffer
    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++) {
        outputBuffer.addFrom(channel, startSample, osc2Buffer, channel, 0, numSamples);
    }
    
    if (!adsr.isActive()) {
        /*
         FIXME: Resetting the pitch wheel here produces a bug where pressing a note while the pitch wheel is already held at a certain non-zero value
         makes it so that the pitch wheel doesn't have an effect until the wheel is moved.
         A solution could be adjusting the frequency in `pitchWheelMoved` but I don't like that.
         */
        pitchWheelDetuneSemitones = 0;
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
    
    // 2.1 Get multiplier from the filter envelope. At 100% amplitude, it controls the cutoff from x0 to x1
    auto adsrMultiplier = juce::jmap(avgFiltAdsrOut, 0.0f, 1.0f, 1.0f - filterEnvDepth, 1.0f);
    // 2.2 Get multiplier from LFO. At 100% amplitude, it can either x.25 or x4 the cutoff
    float lfoMultiplier = pow(juce::jmap(lfoCutoffDepth, 0.0f, 1.0f, 1.0f, 4.0f), lfoOut);
    
    return juce::jmin(juce::jmax(baseCutoffHz * adsrMultiplier * lfoMultiplier, 20.0f), 20000.0f);
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

void SynthVoice::setOscDetune(int semitones, int cents, int oscNum) {
    if (oscNum == 1) {
        osc1DetuneSemitones = ((float) cents / 100.0);
    } else if (oscNum == 2) {
        osc2DetuneSemitones = semitones + ((float) cents / 100.0);
    }
}

void SynthVoice::setOscSineLevel(float percent) {
    sineLevel = percent;
    sineOsc.setLevel(currentVelocity * percent);
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

void SynthVoice::setLFOParams(int lfoShapeId, float ampPercent, float rateHz, int lfoIdToModify) {
    auto& lfoToModify = lfoIdToModify == 1 ? lfo : vibratoLfo;
    
    switch (lfoShapeId) {
        case 0: // Saw up
            lfoToModify.initialise([] (float x) { return juce::jmap (x, -juce::MathConstants<float>::pi, juce::MathConstants<float>::pi, -1.0f, 1.0f); }, 2);
            break;
        case 1: // Saw down
            lfoToModify.initialise([] (float x) { return juce::jmap (x, -juce::MathConstants<float>::pi, juce::MathConstants<float>::pi, 1.0f, -1.0f); }, 2);
            break;
        case 2: // Tri
            lfoToModify.initialise([](float x) { // triangle
                if (x < 0.0f) {
                    return juce::jmap (x, -juce::MathConstants<float>::pi, 0.0f, -1.0f, 1.0f);
                } else {
                    return juce::jmap (x, 0.0f, juce::MathConstants<float>::pi, 1.0f, -1.0f);
                }
            }, 3);
            break;
        case 3: // Square
            lfoToModify.initialise ([] (float x) { return x < 0.0f ? -1.0f : 1.0f; }, 128);
            break;
            // TODO: Add a noise option
        default:
            jassertfalse;
            break;
    }
    
    (lfoIdToModify == 1 ? lfoCutoffDepth : vibratoDepth) = ampPercent;
    lfoToModify.setFrequency(rateHz);
}

// Envelope Module Setters

void SynthVoice::setAmpADSR(float attack, float decay, float sustain, float release) {
    adsr.setParameters(juce::ADSR::Parameters {attack, decay, sustain, release});
}

void SynthVoice::setFilterADSR(float attack, float decay, float sustain, float release) {
    filterAdsr.setParameters(juce::ADSR::Parameters {attack, decay, sustain, release});
}
