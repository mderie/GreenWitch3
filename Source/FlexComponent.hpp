
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

struct FlexBoxPanel; // Forward...

// Aggregate a FlexItem
struct FlexBoxItem : public juce::Component, public TextEditor::Listener, public ComboBox::Listener, public Button::Listener
{
	FlexBoxItem(juce::Colour col, FlexItem& item, int panel, bool in, FlexBoxPanel *parent);
	void AddTextEditor(TextEditor &txt, const juce::String &name, juce::Rectangle<int> b);
	void AddLabel(const String &name, Component &target);
	void AddComboBox(ComboBox &cbo, const juce::String &name, juce::Rectangle<int> b);
	int ComboBoxFindItem(ComboBox &cbo, const juce::String &value);
	std::string CheckBoxToButton(const juce::String &name);
	std::string ButtonToCheckBox(const juce::String &name);
	void AddCheckBox(ToggleButton &chk, const juce::String &name, juce::Rectangle<int> b);
	void AddLabel(Label &lbl, const juce::String value, juce::Rectangle<int> b);
	void AddImageButton(ImageButton &btn, juce::String hint, juce::Rectangle<int> b);
	void AddTextButton(TextButton &btn, const juce::String &name, const juce::String value, juce::String hint, juce::Rectangle<int> b);
	void DrawActivityLed();
	virtual void textEditorTextChanged(TextEditor &sender);
	virtual void comboBoxChanged(ComboBox *sender);
	virtual void buttonClicked(Button* sender);
	void DisableSolo();
	void Copy();
	void Paste();
	void Reset();
	void refreshLayout();
	void paint(Graphics& g) override;

    FlexItem &m_flexItem; // Care, "only" a ref !
	FlexBoxPanel *m_parent;

	Label lblInOut, lblMute, lblSolo, lblSE, lblRS, lblAT, lblRF, lblAB, lblSplitAB, lblCD, lblSplitCD, lblEF, lblSplitEF;
	ComboBox cboDevice;
	TextEditor txtNibbleA, txtNibbleB, txtNibbleC, txtNibbleD, txtNibbleE, txtNibbleF, txtByteAB, txtByteCD, txtByteEF;
	ToggleButton chkSplitAB, chkSplitCD, chkSplitEF, chkMute, chkSolo, chkSE, chkRS, chkAT, chkRF;
	TextButton btnClearDevice;
	TextButton btnCopy, btnPaste, btnReset;
	//ImageButton btnCopy, btnPaste, btnReset;
	Label lblActivity; // Use an imageButton instead ?
	ComponentAnimator fifo; // Fade in Fade out !-)

	int m_panel;
	bool m_in;

    juce::Colour m_colour;
    //OwnedArray<Label> labels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlexBoxItem)
};

// Aggregate a FlexBox
struct FlexBoxPanel : public juce::Component
{
	FlexBoxPanel(int itemCount, int width, int height, int colorIn, int colorOut);
    void resized() override;
    juce::Rectangle<float> getFlexBoxBounds() const;
    void paint(Graphics& g) override;
	void setupFlexBoxItems(int itemCount, int colorIn, int colorOut);
	void addItem(Colour colour, int index, bool in);	
	void OnlyOneSolo(int panel, bool in);
	void Blink(int panel, bool in); // Thread safe !
	void AvailableDevice(const std::string &name, bool in);
	void UnavailableDevice(const std::string &name, bool in);

	FlexBox m_flexBox; // Here we have the 1 - N relation
	OwnedArray<FlexBoxItem> m_items; // That's why the items have only a reference to a FlexItem instance

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlexBoxPanel)
};
