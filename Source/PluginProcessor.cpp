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
    
    juce::NormalisableRange<float> attackRange {0.0001, 10.0, 0.0001}; // sec
    attackRange.setSkewForCentre(0.41);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_ATK", 1), "Amp Attack", attackRange, 0.0004));
    
    juce::NormalisableRange<float> decayRange {0.0001, 10.0, 0.0001}; // sec
    decayRange.setSkewForCentre(0.41);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_DEC", 1), "Amp Decay", decayRange, 0.520));
    
    juce::NormalisableRange<float> sustainRange {1, 100, 1}; // %
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_SUS", 1), "Amp Sustain", sustainRange, 75));
    
    juce::NormalisableRange<float> releaseRange {0.001, 10.0, 0.001}; // sec
    releaseRange.setSkewForCentre(0.5);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_REL", 1), "Amp Release", releaseRange, 0.038));
    
    juce::NormalisableRange<float> gainRange {-84.0, 12.0, 0.1};
    gainRange.setSkewForCentre(-9.0);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("GAIN", 1), "Global Gain", gainRange, 0.0));
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("OSC1_WAVE", 1), "Oscillator 1 Waveform", juce::StringArray {"Saw", "Square", "Noise"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("OSC2_WAVE", 1), "Oscillator 2 Waveform", juce::StringArray {"Saw", "Square", "Triangle"}, 0));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("OSC1_GAIN_RATIO", 1), "Oscillator 1 Gain Ratio", 0.0, 1.0, 1.0));
    
    return { params.begin(), params.end() };
}

void CapstoneSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Clear garbage from buffers for when # outputs > # inputs. Otherwise feedback!
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    // Update each voice with current values from our parameters
    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            auto& attack = *apvts.getRawParameterValue("AMP_ATK");
            auto& decay = *apvts.getRawParameterValue("AMP_DEC");
            double sustain = (*apvts.getRawParameterValue("AMP_SUS")) / 100.0;
            auto& release = *apvts.getRawParameterValue("AMP_REL");
            
            auto& gain = *apvts.getRawParameterValue("GAIN");
            
            auto& osc1Choice = *apvts.getRawParameterValue("OSC1_WAVE");
            auto& osc2Choice = *apvts.getRawParameterValue("OSC2_WAVE");
            
            auto& osc1GainRatio = *apvts.getRawParameterValue("OSC1_GAIN_RATIO");
            
            voice->updateADSR(attack.load(), decay.load(), sustain, release.load());
//            voice->updateGain(gain.load());
            voice->setOscWaveform(osc1Choice.load(), 1);
            voice->setOscWaveform(osc2Choice.load(), 2);
            voice->setOscGainRatios(osc1GainRatio.load());
        }
    }
    
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
    return true; // (change this to false if you choose to not supply an editor)
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
