#include "PluginProcessor.h"
#include "PluginEditor.h"

FioreAudioProcessor::FioreAudioProcessor()
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

FioreAudioProcessor::~FioreAudioProcessor() {}

void FioreAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout FioreAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Oscillator Module Params
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("OSC1_WAVE", 1), "Oscillator 1 Waveform", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("OSC2_WAVE", 1), "Oscillator 2 Waveform", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("OSC1_GAIN_RATIO", 1), "Oscillator 1 Gain Ratio", 0.0, 1.0, 1.0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("DETUNE_CENTS_1", 1), "Osc. 1 Detune (Cents)", -50, 50, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("DETUNE_CENTS", 1), "Osc. 2 Detune (Cents)", -50, 50, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("DETUNE_SEMI", 1), "Osc. 2 Detune (Semitones)", -24, 24, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("SINE_LVL", 1), "Sine Osc. Level", 0, 100, 0));
    
    // Filter Module Params
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("FILT_TYPE", 1), "Filter Type", juce::StringArray {"LPF12", "HPF12", "BPF12", "LPF24", "HPF24", "BPF24"}, 0));
    juce::NormalisableRange<float> cutoffRange {20.0, 10000.0, 1.0};
    cutoffRange.setSkewForCentre(500.0);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_CUTOFF", 1), "Cutoff", cutoffRange, 1000.0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("FILT_RESO", 1), "Resonance", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("FILT_DRIVE_AMT", 1), "Filter Drive Amount", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("FILT_ENV_AMT", 1), "Filter Envelope Amount", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID("FILT_ON_OFF", 1), "Filter On/Off", true));
    
    // Amp Module Params
    juce::NormalisableRange<float> gainRange {-84.0, 12.0, 0.1};
    gainRange.setSkewForCentre(-9.0);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("GAIN", 1), "Global Gain", gainRange, 0.0));
    
    // LFO/Vibrato Module Params
    juce::NormalisableRange<float> rateRange {0.01, 200.0, 0.01};
    rateRange.setSkewForCentre(15.0);
    juce::StringArray lfoShapeOptions { "Saw Up", "Saw Down", "Tri", "Square"};
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("LFO_SHAPE", 1), "Filter LFO Shape", lfoShapeOptions, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("LFO_AMOUNT", 1), "Filter LFO Amount", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("LFO_RATE", 1), "Filter LFO Rate", rateRange, 1.0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("VIB_SHAPE", 1), "Vibrato Shape", lfoShapeOptions, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("VIB_AMOUNT", 1), "Vibrato Amount", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("VIB_RATE", 1), "Vibrato Rate", rateRange, 5.0));
    
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

void FioreAudioProcessor::updateParams() {
    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            
            // Oscillator Module Params
            auto& osc1Choice = *apvts.getRawParameterValue("OSC1_WAVE");
            auto& osc2Choice = *apvts.getRawParameterValue("OSC2_WAVE");
            auto& osc1GainRatio = *apvts.getRawParameterValue("OSC1_GAIN_RATIO");
            auto& osc1DetuneCents = *apvts.getRawParameterValue("DETUNE_CENTS_1");
            auto& osc2DetuneCents = *apvts.getRawParameterValue("DETUNE_CENTS");
            auto& osc2DetuneSemi = *apvts.getRawParameterValue("DETUNE_SEMI");
            double sineLevel = *apvts.getRawParameterValue("SINE_LVL") / 100.0;
            voice->setOscWaveform(osc1Choice.load(), 1);
            voice->setOscWaveform(osc2Choice.load(), 2);
            voice->setOscGainRatios(osc1GainRatio.load());
            voice->setOscDetune(0, osc1DetuneCents, 1);
            voice->setOscDetune(osc2DetuneSemi, osc2DetuneCents, 2);
            voice->setOscSineLevel(sineLevel);
            
            // Filter Module Params
            auto& filtType = *apvts.getRawParameterValue("FILT_TYPE");
            auto& filtCutoff = *apvts.getRawParameterValue("FILT_CUTOFF");
            double filtReso = *apvts.getRawParameterValue("FILT_RESO") / 100.0;
            double filtDriveAmt = *apvts.getRawParameterValue("FILT_DRIVE_AMT") / 100.0;
            double filtEnvAmt = *apvts.getRawParameterValue("FILT_ENV_AMT") / 100.0;
            bool filtOn = *apvts.getRawParameterValue("FILT_ON_OFF");
            voice->setFilterType(filtType.load());
            voice->setFilterParams(filtCutoff.load(), filtReso, filtDriveAmt, filtEnvAmt);
            voice->setFilterOnOff(filtOn);
            
            // Amp Module Params
            auto& gain = *apvts.getRawParameterValue("GAIN");
            voice->setMasterGain(gain.load());
            
            // LFO/Vibrato Module Params
            auto& lfoShape = *apvts.getRawParameterValue("LFO_SHAPE");
            double lfoAmount = *apvts.getRawParameterValue("LFO_AMOUNT") / 100.0;
            auto& lfoRate = *apvts.getRawParameterValue("LFO_RATE");
            auto& vibratoShape = *apvts.getRawParameterValue("VIB_SHAPE");
            double vibratoAmount = *apvts.getRawParameterValue("VIB_AMOUNT") / 100.0;
            auto& vibratoRate = *apvts.getRawParameterValue("VIB_RATE");
            voice->setLFOParams(lfoShape, lfoAmount, lfoRate, 1);
            voice->setLFOParams(vibratoShape, vibratoAmount, vibratoRate, 2);
            
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

void FioreAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
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

void FioreAudioProcessor::releaseResources() {
    
}

juce::AudioProcessorValueTreeState& FioreAudioProcessor::getAPVTS() {
    return apvts;
}

//==============================================================================
// JUCE Auto-Generated Stuff (don't touch)

const juce::String FioreAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool FioreAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FioreAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FioreAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FioreAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int FioreAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FioreAudioProcessor::getCurrentProgram() {
    return 0;
}

void FioreAudioProcessor::setCurrentProgram (int index) {
}

const juce::String FioreAudioProcessor::getProgramName (int index) {
    return {};
}

void FioreAudioProcessor::changeProgramName (int index, const juce::String& newName) {
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FioreAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
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

bool FioreAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* FioreAudioProcessor::createEditor() {
    return new Editor (*this);
}

void FioreAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    // From juce tutorial: https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void FioreAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // From juce tutorial: https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName (apvts.state.getType())) {
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new FioreAudioProcessor();
}
