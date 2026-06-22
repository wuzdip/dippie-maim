/*
  ==============================================================================

    Mp3ControllerManager.h
    Created: 10 Apr 2023 5:05:43pm
    Author:  Arden Butterfield

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#include "CodecControllers/BladeController.h"
#include "CodecControllers/LameController.h"
#include "CodecControllers/OpusController.h"
#include "parameterIds.h"
#include "RateGate.h"

#define MP3FRAMESIZE 1152
enum Encoder {
    blade = 0,
    lame = 1,
    opus = 2
};

class Mp3ControllerManager : public juce::AudioProcessorValueTreeState::Listener,
public juce::Timer
{
public:
    explicit Mp3ControllerManager(juce::AudioProcessorValueTreeState& parameters);
    ~Mp3ControllerManager() override;


    void initialize(int samplerate, int initialBitrate, int samplesPerBlock);

    void processBlock(juce::AudioBuffer<float>& buffer, double bpm);
    
    int getBitrate();
    
    float* getPsychoanalEnergy();
    float* getPsychoanalThreshold();
    float* getMDCTpreBend();
    float* getMDCTpostBend();
    
    static constexpr std::array<int, 17> bitrates {
        8,
        16,
        24,
        32,
        40,
        48,
        56,
        64,
        80,
        96,
        112,
        128,
        160,
        192,
        224,
        256,
        320
    };

    
private:
    void timerCallback() override;
    
    std::atomic<bool> parametersNeedUpdating;
    void parameterChanged (const juce::String &parameterID, float newValue) override;
    void updateParameters();
    void changeController(int bitrate, Encoder encoder);
    bool wantingToSwitch;
    
    int currentBitrate;
    int desiredBitrate;
    Encoder currentEncoder;
    Encoder desiredEncoder;
    
    int currentControllerIndex;
    
    int samplerate;
    int samplesPerBlock;
    
    int blocksBeforeSwitch;
    int switchCountdown;

    std::unique_ptr<QueueBuffer<float>> inputBufferL;
    std::unique_ptr<QueueBuffer<float>> inputBufferR;

    std::unique_ptr<QueueBuffer<float>> outputBufferL;
    std::unique_ptr<QueueBuffer<float>> outputBufferR;

    float previousFrames[2][2][1152];
    
    std::array<LameController, 2> lameControllers;
    std::array<BladeController, 2> bladeControllers;
    CodecController* currentController;
    CodecController* offController;
    
    std::array<juce::AudioParameterInt*, 20> bandReassignmentParameters;
    juce::AudioProcessorValueTreeState& parameters;

    float lastDecodedFrame[2][MP3FRAMESIZE] {};
    float ghostFrame[2][MP3FRAMESIZE] {};

    // Speed: tempo-syncable freeze gate. Knob value 1 = always fresh (normal),
    // 0 = always frozen on lastDecodedFrame, in both free and synced modes.
    RateGate speedGate;
    bool prevSpeedFrozen { false };

    // Stutter: tempo-syncable buffer-repeat gate. On entering its "active"
    // phase, captures a chunk of decoded output and loops it for the rest
    // of that phase, then releases back to fresh audio.
    RateGate stutterGate;
    std::vector<float> stutterCaptureL, stutterCaptureR;
    int stutterCaptureTarget { 0 };
    int stutterCapturedLength { 0 };
    int stutterPlaybackPos { 0 };
    bool stutterCapturing { false };
    bool prevStutterActive { false };

    // Smooth: short crossfade applied at Speed/Stutter discontinuity
    // boundaries, ramping from the previous frame's true tail into the new
    // frame's content, to avoid audible clicks from the hard frame swaps.
    static constexpr int maxSmoothRampSamples = 480;
    float previousOutputTail[2][maxSmoothRampSamples] {};
    void applySmoothing(float frameOut[2][MP3FRAMESIZE], bool discontinuity);
    void captureOutputTail(const float frameOut[2][MP3FRAMESIZE]);
};
