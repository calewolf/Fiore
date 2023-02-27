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
                       )
#endif
{
    for (auto i = 0; i < 8; ++i) {
        synth.addVoice(new SineWaveVoice());
    }
        
    synth.addSound(new SineWaveSound());
}

CapstoneSynthAudioProcessor::~CapstoneSynthAudioProcessor() {}

void CapstoneSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void CapstoneSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Included to clear garbage from buffers for when # outputs > # inputs
    // Prevents crazy loud feedback
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void CapstoneSynthAudioProcessor::releaseResources() {
    
}

juce::MidiKeyboardState* CapstoneSynthAudioProcessor::getMidiKeyboardState() {
    return &keyboardState;
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
