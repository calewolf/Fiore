#include "PluginProcessor.h"
#include "PluginEditor.h"

CapstoneSynthAudioProcessor::CapstoneSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParams())
#endif
{
    for (int i = 0; i < 16; i++) {
        synth.addVoice(new SynthVoice());
    }
    synth.addSound(new SynthSound());
    synth.setNoteStealingEnabled(true);
}

CapstoneSynthAudioProcessor::~CapstoneSynthAudioProcessor() {}

void CapstoneSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout CapstoneSynthAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Oscillator Module Params
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("OSC1_WAVE", 1), "Oscillator 1 Waveform", juce::StringArray {"Saw", "Square", "Noise"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("OSC2_WAVE", 1), "Oscillator 2 Waveform", juce::StringArray {"Saw", "Square", "Triangle"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("OSC1_GAIN_RATIO", 1), "Oscillator 1 Gain Ratio", 0.0, 1.0, 1.0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("DETUNE_CENTS", 1), "Osc. 2 Detune (Cents)", -50, 50, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("DETUNE_SEMI", 1), "Osc. 2 Detune (Semitones)", -24, 24, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("VIBRATO", 1), "Vibrato Depth", 0.0, 36.0, 0.0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("SINE_LVL", 1), "Sine Osc. Level", 0.0, 1.0, 0.0));
    
    // Filter Module Params
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("FILT_TYPE", 1), "Filter Type", juce::StringArray {"Lowpass", "Highpass", "Bandpass"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_CUTOFF", 1), "Cutoff", 0.0, 1.0, 0.0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_RESO", 1), "Resonance", 0.0, 1.0, 0.0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_LFO_AMT", 1), "Filter LFO Amount", 0.0, 1.0, 0.0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_ENV_AMT", 1), "Filter Envelope Amount", 0.0, 1.0, 0.0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID("FILT_ON_OFF", 1), "Filter On/Off", true));
    
    // Amp Module Params
    juce::NormalisableRange<float> gainRange {-84.0, 12.0, 0.1};
    gainRange.setSkewForCentre(-9.0);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("GAIN", 1), "Global Gain", gainRange, 0.0));
    
    // LFO/Vibrato Module Params
    // TODO: LFO shape, LFO percent, LFO hz, Vibrato shape, Vibrato pct, Vib hz
    
    // ADSR Module Params
    juce::NormalisableRange<float> attackRange {0.0001, 10.0, 0.0001}; // sec
    juce::NormalisableRange<float> decayRange {0.0001, 10.0, 0.0001}; // sec
    juce::NormalisableRange<float> sustainRange {1, 100, 1}; // %
    juce::NormalisableRange<float> releaseRange {0.001, 10.0, 0.001}; // sec
    attackRange.setSkewForCentre(0.41);
    decayRange.setSkewForCentre(0.41);
    releaseRange.setSkewForCentre(0.5);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_ATK", 1), "Amp Attack", attackRange, 0.0004));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_DEC", 1), "Amp Decay", decayRange, 0.520));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_SUS", 1), "Amp Sustain", sustainRange, 75));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_REL", 1), "Amp Release", releaseRange, 0.038));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_ATK", 1), "Filter Attack", attackRange, 0.0004));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_DEC", 1), "Filter Decay", decayRange, 0.520));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_SUS", 1), "Filter Sustain", sustainRange, 75));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_REL", 1), "Filter Release", releaseRange, 0.038));
    
    return { params.begin(), params.end() };
}

void CapstoneSynthAudioProcessor::updateParams() {
    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            
            // Oscillator Module Params
            auto& osc1Choice = *apvts.getRawParameterValue("OSC1_WAVE");
            auto& osc2Choice = *apvts.getRawParameterValue("OSC2_WAVE");
            auto& osc1GainRatio = *apvts.getRawParameterValue("OSC1_GAIN_RATIO");
            auto& osc2DetuneCents = *apvts.getRawParameterValue("DETUNE_CENTS");
            auto& osc2DetuneSemi = *apvts.getRawParameterValue("DETUNE_SEMI");
            auto& vibratoDepth = *apvts.getRawParameterValue("VIBRATO");
            auto& sineLevel = *apvts.getRawParameterValue("SINE_LVL");
            voice->setOscWaveform(osc1Choice.load(), 1);
            voice->setOscWaveform(osc2Choice.load(), 2);
            voice->setOscGainRatios(osc1GainRatio.load());
            voice->setOscDetune(osc2DetuneSemi, osc2DetuneCents);
            voice->setOscVibratoDepth(vibratoDepth.load());
            voice->setOscSineLevel(sineLevel.load());
            
            // Filter Module Params
            auto& filtType = *apvts.getRawParameterValue("FILT_TYPE");
            auto& filtCutoff = *apvts.getRawParameterValue("FILT_CUTOFF");
            auto& filtReso = *apvts.getRawParameterValue("FILT_RESO");
            auto& filtLfoAmt = *apvts.getRawParameterValue("FILT_LFO_AMT");
            auto& filtEnvAmt = *apvts.getRawParameterValue("FILT_ENV_AMT");
            auto& filtOn = *apvts.getRawParameterValue("FILT_ON_OFF");
            voice->setFilterType(filtType.load());
            voice->setFilterParams(filtCutoff.load(), filtReso.load(), filtLfoAmt.load(), filtEnvAmt.load());
            voice->setFilterOnOff(filtOn);
            
            // Amp Module Params
            auto& gain = *apvts.getRawParameterValue("GAIN");
            voice->setMasterGain(gain.load());
            
            // LFO/Vibrato Module Params
            // TODO: Add in LFO/Vibrato module params
            
            // ADSR Module Params
            auto& ampAtk = *apvts.getRawParameterValue("AMP_ATK");
            auto& ampDecay = *apvts.getRawParameterValue("AMP_DEC");
            double ampSus = (*apvts.getRawParameterValue("AMP_SUS")) / 100.0;
            auto& ampRel = *apvts.getRawParameterValue("AMP_REL");
            auto& filtAtk = *apvts.getRawParameterValue("FILT_ATK");
            auto& filtDecay = *apvts.getRawParameterValue("FILT_DEC");
            double filtSus = (*apvts.getRawParameterValue("FILT_SUS")) / 100.0;
            auto& filtRel = *apvts.getRawParameterValue("FILT_REL");
            voice->setAmpADSR(ampAtk.load(), ampDecay.load(), ampSus, ampRel.load());
            voice->setFilterADSR(filtAtk.load(), filtDecay.load(), filtSus, filtRel.load());
        }
    }
}

void CapstoneSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Clear garbage from buffers for when # outputs > # inputs. Otherwise feedback!
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    // Update each voice with current values from our parameters
    // TODO: Figure out a way to save memory by only calling updates when needed!
    updateParams();
    
    // Get audio from the synth. Will call renderNextBlock for all the synth voices
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void CapstoneSynthAudioProcessor::releaseResources() {
    
}

juce::AudioProcessorValueTreeState& CapstoneSynthAudioProcessor::getAPVTS() {
    return apvts;
}

//==============================================================================
// JUCE Auto-Generated Stuff (don't touch)

const juce::String CapstoneSynthAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool CapstoneSynthAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CapstoneSynthAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CapstoneSynthAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CapstoneSynthAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int CapstoneSynthAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CapstoneSynthAudioProcessor::getCurrentProgram() {
    return 0;
}

void CapstoneSynthAudioProcessor::setCurrentProgram (int index) {
}

const juce::String CapstoneSynthAudioProcessor::getProgramName (int index) {
    return {};
}

void CapstoneSynthAudioProcessor::changeProgramName (int index, const juce::String& newName) {
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CapstoneSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

bool CapstoneSynthAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* CapstoneSynthAudioProcessor::createEditor() {
    return new Editor (*this);
}

void CapstoneSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CapstoneSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new CapstoneSynthAudioProcessor();
}
