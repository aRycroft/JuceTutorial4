/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LimiterAudioProcessor::LimiterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

LimiterAudioProcessor::~LimiterAudioProcessor()
{
}

//==============================================================================
const String LimiterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LimiterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LimiterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LimiterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LimiterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LimiterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LimiterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LimiterAudioProcessor::setCurrentProgram (int index)
{
}

const String LimiterAudioProcessor::getProgramName (int index)
{
    return {};
}

void LimiterAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void LimiterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    delayBuffer = AudioSampleBuffer(1, 10);
    delayBuffer.clear();

    limiterThresh = 0.01f;
    gain = 1.0f;
    xPeak = 0.0f;
}

void LimiterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LimiterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void LimiterAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    auto* data = buffer.getWritePointer(0);
    float attackTime, releaseTime, coeff;
    attackTime = 0.3f;
    releaseTime = 0.01f;
    
    int writeIndex, readIndex, delayIndex, bufferLength;
    delayIndex = 2;
    writeIndex = 0;
    bufferLength = 10;

    for (int i = 0; i < buffer.getNumSamples(); i++) {

        float sample = data[i];

        float amplitude = abs(sample);

        if (amplitude > xPeak) coeff = attackTime;
        else coeff = releaseTime;

        xPeak = (1 - coeff) * xPeak + coeff * amplitude;

        float filter = fmin(1.0f, limiterThresh / xPeak);

        if (gain > filter) coeff = attackTime;
        else coeff = releaseTime;

        gain = (1 - coeff) * gain + coeff * filter;

        readIndex = ((bufferLength + writeIndex) - delayIndex) % bufferLength;
        
        float limitedSample = gain * delayBuffer.getSample(0, readIndex);
        delayBuffer.setSample(0, writeIndex, sample);
       
        writeIndex = (writeIndex + 1) % bufferLength;
        
        data[i] = limitedSample;
    }
}

//==============================================================================
bool LimiterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LimiterAudioProcessor::createEditor()
{
    return new LimiterAudioProcessorEditor (*this);
}

//==============================================================================
void LimiterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LimiterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LimiterAudioProcessor();
}
