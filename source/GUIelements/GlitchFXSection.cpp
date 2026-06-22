//
// GlitchFXSection.cpp
//

#include "GlitchFXSection.h"
#include "../parameterIds.h"

GlitchFXSection::GlitchFXSection(juce::AudioProcessorValueTreeState& p) :
    StageWindow(p),
    speedSlider(p, FRAME_SPEED_PARAM_ID, "Speed"),
    stutterSlider(p, STUTTER_PARAM_ID, "Stutter"),
    pitchSlider(p, MDCT_PITCH_SHIFT_PARAM_ID, "Pitch"),
    smoothSlider(p, SMOOTH_PARAM_ID, "Smooth"),
    ghostSlider(p, SPECTRAL_GHOST_PARAM_ID, "Ghost"),
    speedRateSlider(p, SPEED_RATE_PARAM_ID, "Rate"),
    stutterRateSlider(p, STUTTER_RATE_PARAM_ID, "Rate"),
    speedSyncAttachment(p, SPEED_SYNC_PARAM_ID, speedSyncToggle),
    speedRateSyncedAttachment(p, SPEED_RATE_SYNCED_PARAM_ID, speedRateSyncedBox),
    stutterSyncAttachment(p, STUTTER_SYNC_PARAM_ID, stutterSyncToggle),
    stutterRateSyncedAttachment(p, STUTTER_RATE_SYNCED_PARAM_ID, stutterRateSyncedBox),
    parameters(p)
{
    sectionName.setColour(sectionName.textColourId, MaimColours::BEVEL_BLACK);
    sectionName.setFont(sectionNameFont);
    sectionName.setText("Glitch FX", juce::dontSendNotification);
    sectionName.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sectionName);

    for (auto* knob : { &speedSlider, &stutterSlider, &pitchSlider, &smoothSlider, &ghostSlider }) {
        knob->setFontSize(12.f);
        addAndMakeVisible(knob);
    }
    for (auto* knob : { &speedRateSlider, &stutterRateSlider }) {
        knob->setFontSize(10.f);
        knob->slider.setTextValueSuffix(" Hz");
        addAndMakeVisible(knob);
    }

    for (int i = 0; i < SYNCED_RATE_DIVISIONS.size(); ++i) {
        speedRateSyncedBox.addItem(SYNCED_RATE_DIVISIONS[i], i + 1);
        stutterRateSyncedBox.addItem(SYNCED_RATE_DIVISIONS[i], i + 1);
    }
    addAndMakeVisible(speedRateSyncedBox);
    addAndMakeVisible(stutterRateSyncedBox);

    addAndMakeVisible(speedSyncToggle);
    addAndMakeVisible(stutterSyncToggle);

    parameters.addParameterListener(SPEED_SYNC_PARAM_ID, this);
    parameters.addParameterListener(STUTTER_SYNC_PARAM_ID, this);

    bool speedSynced = ((juce::AudioParameterBool*) parameters.getParameter(SPEED_SYNC_PARAM_ID))->get();
    speedRateSlider.setVisible(!speedSynced);
    speedRateSyncedBox.setVisible(speedSynced);

    bool stutterSynced = ((juce::AudioParameterBool*) parameters.getParameter(STUTTER_SYNC_PARAM_ID))->get();
    stutterRateSlider.setVisible(!stutterSynced);
    stutterRateSyncedBox.setVisible(stutterSynced);
}

GlitchFXSection::~GlitchFXSection()
{
    parameters.removeParameterListener(SPEED_SYNC_PARAM_ID, this);
    parameters.removeParameterListener(STUTTER_SYNC_PARAM_ID, this);
}

void GlitchFXSection::parameterChanged(const juce::String& parameterID, float newValue)
{
    bool synced = newValue > 0.5f;
    if (parameterID == SPEED_SYNC_PARAM_ID) {
        speedRateSlider.setVisible(!synced);
        speedRateSyncedBox.setVisible(synced);
    } else if (parameterID == STUTTER_SYNC_PARAM_ID) {
        stutterRateSlider.setVisible(!synced);
        stutterRateSyncedBox.setVisible(synced);
    }
}

void GlitchFXSection::paint(juce::Graphics& g)
{
    StageWindow::paint(g);
}

void GlitchFXSection::layoutSyncedKnob(juce::Rectangle<int> column,
                                       NamedRotarySlider& mainKnob,
                                       juce::ToggleButton& syncToggle,
                                       NamedRotarySlider& freeRateKnob,
                                       juce::ComboBox& syncedRateBox)
{
    auto knobArea = column.removeFromTop(column.getHeight() * 2 / 3);
    mainKnob.setBounds(knobArea);

    auto rateRow = column;
    syncToggle.setBounds(rateRow.removeFromLeft(rateRow.getWidth() / 3));
    freeRateKnob.setBounds(rateRow);
    syncedRateBox.setBounds(rateRow.withHeight(24).withCentre(rateRow.getCentre()));
}

void GlitchFXSection::resized()
{
    const int titleHeight = headerHeight;

    auto mainPart = getLocalBounds()
                        .withTrimmedLeft(standardMargin)
                        .withTrimmedRight(standardMargin)
                        .withTrimmedBottom(standardMargin);

    sectionName.setBounds(mainPart.withHeight(titleHeight));
    mainPart = mainPart.withTrimmedTop(titleHeight);

    const int numColumns = 5;
    const int columnWidth = mainPart.getWidth() / numColumns;

    auto columnAt = [&](int index) {
        return mainPart.withWidth(columnWidth).withX(mainPart.getX() + index * columnWidth);
    };

    layoutSyncedKnob(columnAt(0), speedSlider, speedSyncToggle, speedRateSlider, speedRateSyncedBox);
    layoutSyncedKnob(columnAt(1), stutterSlider, stutterSyncToggle, stutterRateSlider, stutterRateSyncedBox);
    pitchSlider.setBounds(columnAt(2).removeFromTop(mainPart.getHeight() * 2 / 3));
    smoothSlider.setBounds(columnAt(3).removeFromTop(mainPart.getHeight() * 2 / 3));
    ghostSlider.setBounds(columnAt(4).removeFromTop(mainPart.getHeight() * 2 / 3));
}
