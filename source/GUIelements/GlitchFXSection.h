//
// GlitchFXSection.h
//
// Bottom row of tempo-syncable glitch effects: Speed, Stutter, Pitch,
// Smooth, Ghost. Speed and Stutter each get a Sync toggle that switches
// their Rate control between a free-running Hz knob and a synced musical
// division chooser.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>

#include "NamedRotarySlider.h"
#include "StageWindow.h"

class GlitchFXSection : public StageWindow, public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit GlitchFXSection(juce::AudioProcessorValueTreeState& p);
    ~GlitchFXSection() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void layoutSyncedKnob(juce::Rectangle<int> column,
                           NamedRotarySlider& mainKnob,
                           juce::ToggleButton& syncToggle,
                           NamedRotarySlider& freeRateKnob,
                           juce::ComboBox& syncedRateBox);

    const juce::Font sectionNameFont = juce::Font(MaimLookAndFeel().main_font).withHeight(20.f);
    juce::Label sectionName;

    NamedRotarySlider speedSlider;
    NamedRotarySlider stutterSlider;
    NamedRotarySlider pitchSlider;
    NamedRotarySlider smoothSlider;
    NamedRotarySlider ghostSlider;

    juce::ToggleButton speedSyncToggle { "SYNC" };
    NamedRotarySlider speedRateSlider;
    juce::ComboBox speedRateSyncedBox;

    juce::ToggleButton stutterSyncToggle { "SYNC" };
    NamedRotarySlider stutterRateSlider;
    juce::ComboBox stutterRateSyncedBox;

    juce::AudioProcessorValueTreeState::ButtonAttachment speedSyncAttachment;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment speedRateSyncedAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment stutterSyncAttachment;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment stutterRateSyncedAttachment;

    juce::AudioProcessorValueTreeState& parameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlitchFXSection)
};
