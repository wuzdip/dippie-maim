/*
  ==============================================================================

    MiscellaneaSection.cpp
    Created: 7 Apr 2023 9:21:14am
    Author:  Arden Butterfield

  ==============================================================================
*/

#include "MiscellaneaSection.h"
#include "../parameterIds.h"

MiscellaneaSection::MiscellaneaSection (juce::AudioProcessorValueTreeState& p, SpectrogramDataSource& spectrogramSource)
        : StageWindow(p),
            butterflyDragBox(p, BUTTERFLY_STANDARD_PARAM_ID, BUTTERFLY_CROSSED_PARAM_ID),
      mdctDragBox(p, MDCT_STEP_PARAM_ID, MDCT_WINDOW_INCREMENT_PARAM_ID),
      spectrogram(spectrogramSource)
{
    sectionName.setColour(sectionName.textColourId, MaimColours::BEVEL_BLACK);
    sectionName.setFont(sectionNameFont);
    sectionName.setText("Output spectrogram", juce::dontSendNotification);
    sectionName.setJustificationType(juce::Justification::centred);
    lameOnlyLabel.setColour(lameOnlyLabel.textColourId, MaimColours::BEVEL_BLACK);
    lameOnlyLabel.setFont(lameLabelFont);
    lameOnlyLabel.setText("Lame only", juce::dontSendNotification);
    lameOnlyLabel.setJustificationType(juce::Justification::centred);
    butterflyLabel.setColour(sectionName.textColourId, MaimColours::BEVEL_BLACK);
    butterflyLabel.setFont(sectionNameFont.withHeight(11));
    butterflyLabel.setText("Butterfly", juce::dontSendNotification);
    butterflyLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(spectrogram);
    addAndMakeVisible(butterflyDragBox);
    addAndMakeVisible(mdctDragBox);
    addAndMakeVisible(butterflyLabel);
    addAndMakeVisible(sectionName);
    addAndMakeVisible(lameOnlyLabel);
    parameters.addParameterListener(ENCODER_PARAM_ID, this);
    bool isLame = (((juce::AudioParameterChoice*)parameters.getParameter(ENCODER_PARAM_ID))->getIndex() == 1);
    mdctDragBox.setVisible(isLame);
    lameOnlyLabel.setVisible(!isLame);
}

MiscellaneaSection::~MiscellaneaSection()
{
    parameters.removeParameterListener(ENCODER_PARAM_ID,this);
}

void MiscellaneaSection::mouseMove (const juce::MouseEvent& event)
{

}

void MiscellaneaSection::mouseUp (const juce::MouseEvent& event)
{

}

void MiscellaneaSection::paint (juce::Graphics& g)
{
    StageWindow::paint(g);
/*
    g.setColour(MaimColours::BEVEL_LIGHT);
    g.fillRect(lameOnlySection);
    g.setColour(MaimColours::BEVEL_DARK);
    g.drawRect(lameOnlySection, 1);
*/
}

void MiscellaneaSection::resized()
{
    const int titleHeight = headerHeight;

    auto mainPart = getLocalBounds()
                        .withTrimmedLeft(standardMargin)
                        .withTrimmedRight(standardMargin)
                        .withTrimmedBottom(standardMargin);

    sectionName.setBounds(mainPart.withHeight(titleHeight));
    mainPart = mainPart.withTrimmedTop(titleHeight);

    auto spectrogramPart = mainPart.withHeight(mainPart.getHeight() * 2 / 3);
    auto controlsPart = mainPart.withTrimmedTop(spectrogramPart.getHeight() + 5);

    spectrogram.setBounds(spectrogramPart);

    auto butterflyPart = controlsPart.withWidth(controlsPart.getWidth() / 2).withTrimmedRight(5);
    auto mdctPart = controlsPart.withTrimmedLeft(butterflyPart.getWidth() + 5);

    const int labelHeight = 14;
    butterflyLabel.setBounds(butterflyPart.withHeight(labelHeight));
    butterflyDragBox.setBounds(butterflyPart.withTrimmedTop(labelHeight));
    lameOnlyLabel.setBounds(mdctPart.withHeight(labelHeight));
    mdctDragBox.setBounds(mdctPart.withTrimmedTop(labelHeight));
}

void MiscellaneaSection::parameterChanged (const juce::String& parameterID, float newValue)
{
    bool isLame = (newValue == 1);
    mdctDragBox.setVisible(isLame);
    lameOnlyLabel.setVisible(!isLame);
}
