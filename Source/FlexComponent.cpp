
#include "FlexComponent.hpp"
#include "SessionData.hpp"
//#include "MidiMessageData.hpp"
//#include "MidiDeviceData.hpp"
#include "CommonStuffs.hpp"

#include <chrono>
#include <thread>

/////////////////
// FlexBoxItem //
/////////////////

FlexBoxItem::FlexBoxItem(juce::Colour col, FlexItem& item, int panel, bool in, FlexBoxPanel *parent) : m_flexItem(item), m_colour(col)
{
	m_flexItem = item;
	m_colour = col;
	m_parent = parent;
	m_panel = panel;
	m_in = in;

	int x = 3;
	int y = 3;

	juce::String prefix = (in ? "I" : "O") + juce::String(panel) + "_";

	// First line

	AddLabel(lblInOut, (in ? "In" : "Out"), { x, y, 30, 18 });
	x += 32;

	//TODO: Fine tune this...
	//AddTextButton(btnActivity, "", { x, y, 20, 18 });
	//AddImageButton(btnActivity, "", { x, y, 20, 18 });
	AddLabel(lblActivity, "(@)", { x, y, 32, 18 });
	//lblActivity.visible ?
	x += 32;

	// Combo are specials, they holds the midi device names
	std::vector<std::string> deviceNames = SessionSingleton::RetrieveMidiDevices(in);
	int index = 1;
	for (const auto &item : deviceNames)
	{
		cboDevice.addItem(item, index++);
	}
	AddComboBox(cboDevice, prefix + juce::String(controlContents[(int)ControlContent::Device]), { x, y, 240, 18 });
	x += 242;

	// Exception here : we use a regular text button to clear the combo
	AddTextButton(btnClearDevice, "ClearDevice", "X", "Clear device", { x, y, 20, 18 }); // Use "!" ?
	x += 122;

	// Top right panel action icons
	// TODO: Replace TextButton by ImageButton
	//AddImageButton(btnCopy, { x, y, 20, 18 });
	AddTextButton(btnCopy, "Copy", "C", "Copy", { x, y, 20, 18 });
	x += 22;
	//AddImageButton(btnPaste, { x, y, 20, 18 });
	AddTextButton(btnPaste, "Paste", "P", "Paste", { x, y, 20, 18 });
	x += 22;
	//AddImageButton(btnReset, { x, y, 20, 18 });
	AddTextButton(btnReset, "Reset", "R", "Reset", { x, y, 20, 18 }); // Use "X" ?
	x += 22;

	// Second line
	y += 23;
	x = 3;

	AddLabel(lblMute, "Mute", { x, y, 34, 18 });
	x += 36;
	AddCheckBox(chkMute, prefix + juce::String(controlContents[(int)ControlContent::Mute]), { x, y, 20, 18 });
	x += 22;
	AddLabel(lblSolo, "Solo", { x, y, 34, 18 });
	x += 36;
	AddCheckBox(chkSolo, prefix + juce::String(controlContents[(int)ControlContent::Solo]), { x, y, 20, 18 });
	x += 22;
	if (in)
	{
		AddLabel(lblSE, "SE", { x, y, 34, 18 });
		x += 36;
		AddCheckBox(chkSE, prefix + juce::String(controlContents[(int)ControlContent::SE]), { x, y, 20, 18 });
		x += 22;
		AddLabel(lblRS, "RS", { x, y, 34, 18 });
		x += 36;
		AddCheckBox(chkRS, prefix + juce::String(controlContents[(int)ControlContent::RS]), { x, y, 20, 18 });
		x += 22;
		AddLabel(lblAT, "AT", { x, y, 34, 18 });
		x += 36;
		AddCheckBox(chkAT, prefix + juce::String(controlContents[(int)ControlContent::AT]), { x, y, 20, 18 });
		x += 22;
		AddLabel(lblRF, "RF", { x, y, 34, 18 });
		x += 36;
		AddCheckBox(chkRF, prefix + juce::String(controlContents[(int)ControlContent::RF]), { x, y, 20, 18 });
		x += 22;
	}

	// Third line
	y += 23;
	x = 3;

	AddLabel(lblAB, "AB", { x, y, 32, 18 });
	x += 34;
	AddTextEditor(txtByteAB, prefix + juce::String(controlContents[(int)ControlContent::ByteAB]), { x, y, (in ? 80 : 120), 18 });
	x += (in ? 92 : 132);
	AddLabel(lblCD, "CD", { x, y, 32, 18 });
	x += 34;
	AddTextEditor(txtByteCD, prefix + juce::String(controlContents[(int)ControlContent::ByteCD]), { x, y, (in ? 80 : 120), 18 });
	x += (in ? 92 : 132);
	AddLabel(lblEF, "EF", { x, y, 32, 18 });
	x += 34;
	AddTextEditor(txtByteEF, prefix + juce::String(controlContents[(int)ControlContent::ByteEF]), { x, y, (in ? 80 : 120), 18 });
	x += (in ? 92 : 132);

	// Last Line
	y += 23;
	x = 3;

	AddLabel(lblSplitAB, "Split", { x, y, 30, 18 });
	x += 32;
	AddCheckBox(chkSplitAB, prefix + juce::String(controlContents[(int)ControlContent::SplitAB]), { x, y, 20, 18 });
	x += 22;
	AddTextEditor(txtNibbleA, prefix + juce::String(controlContents[(int)ControlContent::NibbleA]), { x, y, (in ? 32 : 52), 18 });
	x += (in ? 34 : 54);
	AddTextEditor(txtNibbleB, prefix + juce::String(controlContents[(int)ControlContent::NibbleB]), { x, y, (in ? 32 : 52), 18 });
	x += (in ? 34 : 54);

	AddLabel(lblSplitCD, "Split", { x, y, 30, 18 });
	x += 32;
	AddCheckBox(chkSplitCD, prefix + juce::String(controlContents[(int)ControlContent::SplitCD]), { x, y, 20, 18 });
	x += 22;
	AddTextEditor(txtNibbleC, prefix + juce::String(controlContents[(int)ControlContent::NibbleC]), { x, y, (in ? 32 : 52), 18 });
	x += (in ? 34 : 54);
	AddTextEditor(txtNibbleD, prefix + juce::String(controlContents[(int)ControlContent::NibbleD]), { x, y, (in ? 32 : 52), 18 });
	x += (in ? 34 : 54);

	AddLabel(lblSplitEF, "Split", { x, y, 30, 18 });
	x += 32;
	AddCheckBox(chkSplitEF, prefix + juce::String(controlContents[(int)ControlContent::SplitEF]), { x, y, 20, 18 });
	x += 22;
	AddTextEditor(txtNibbleE, prefix + juce::String(controlContents[(int)ControlContent::NibbleE]), { x, y, (in ? 32 : 52), 18 });
	x += (in ? 34 : 54);
	AddTextEditor(txtNibbleF, prefix + juce::String(controlContents[(int)ControlContent::NibbleF]), { x, y, (in ? 32 : 52), 18 });
	x += (in ? 34 : 54);
}

void FlexBoxItem::AddTextEditor(TextEditor &txt, const juce::String &name, juce::Rectangle<int> b)
{
	txt.setBounds(b);
	txt.setName(name);
	txt.setText(SessionSingleton::Get(name.toStdString()));
	txt.addListener(this); // Must be done after setText
	addAndMakeVisible(txt);
}

void FlexBoxItem::AddLabel(const String &name, Component &target)
{
	auto label = new Label(name, name);
	label->attachToComponent(&target, true);
	//labels.add(label);
	addAndMakeVisible(label);
}

void FlexBoxItem::AddComboBox(ComboBox &cbo, const juce::String &name, juce::Rectangle<int> b)
{
	cbo.setBounds(b);
	cbo.setName(name);
	juce::String value = SessionSingleton::Get(name.toStdString());
	if (value != "")
	{
		int index = ComboBoxFindItem(cbo, value);
		if (index != -1)
		{
			cbo.setSelectedItemIndex(index); // Async notif by default !
		}
	}
	// Care by default juce sort alphabetically the items !
	// cbo.setTextWhenNothingSelected("Choose a device...");
	cbo.addListener(this);
	addAndMakeVisible(cbo);
}

// Return the Id of the given item if present -1 otherwise
int FlexBoxItem::ComboBoxFindItem(ComboBox &cbo, const juce::String &value)
{
	for (int i = 0; i < cbo.getNumItems(); i++)
	{
		if (cbo.getItemText(i) == value)
		{
			return i;// cbo.getItemId(i);
		}
	}

	return -1;
}

std::string FlexBoxItem::CheckBoxToButton(const juce::String &name)
{
	return (LeftDollar(std::string(name.toStdString()), "_chk") + "_" + RightDollar(std::string(name.toStdString()), "_chk")); // Little beurk...
}

std::string FlexBoxItem::ButtonToCheckBox(const juce::String &name)
{
	return (LeftDollar(std::string(name.toStdString()), "_") + "_chk" + RightDollar(std::string(name.toStdString()), "_")); // Idem
}

void FlexBoxItem::AddCheckBox(ToggleButton &chk, const juce::String &name, juce::Rectangle<int> b)
{
	chk.setBounds(b);
	chk.setName(ButtonToCheckBox(name));
	chk.setToggleState(SessionSingleton::Get(name.toStdString()) == "1", NotificationType::dontSendNotification); // We don't want them that early
	chk.addListener(this);
	addAndMakeVisible(chk);
}

// Not part of the session
void FlexBoxItem::AddLabel(Label &lbl, const juce::String value, juce::Rectangle<int> b)
{
	lbl.setBounds(b);
	lbl.setText(value, NotificationType::dontSendNotification); // It is a label, who needs a notification from them ?
	addAndMakeVisible(lbl);
}

// Not part of the session
void FlexBoxItem::AddImageButton(ImageButton &btn, juce::String hint, juce::Rectangle<int> b)
{
	btn.setBounds(b);

	//TODO: Set two (three ?) images here See in the Juce DemoRunner ("../Assets/DemoUtilities.h")
	//btn.addListener()
	/*
	DrawableImage down;
	down.setImage(getImageFromAssets("juce_icon.png"));
	down.setOverlayColour(Colours::black.withAlpha(0.3f));
	*/

	addAndMakeVisible(btn);
}

// Not part of the session
void FlexBoxItem::AddTextButton(TextButton &btn, const juce::String &name, const juce::String value, juce::String hint, juce::Rectangle<int> b)
{
	btn.setBounds(b);
	btn.setName(name);
	btn.setTooltip(hint); //TODO: Use JUCE\modules\juce_gui_basics\windows\juce_TooltipWindow.h 
	btn.setButtonText(value);
	btn.addListener(this);
	addAndMakeVisible(btn);
}

// Not part of the session
// Make it flashing when midi message pass through the corresponding device
void FlexBoxItem::DrawActivityLed()
{
	MessageManagerLock mml; // Avoid thread interlocking : allow callbacks originating from the midi devices

	/* Does not work...
	lblActivity.setColour(0, Colours::pink);
	//lblActivity.paint(?); Paint shall not be called directly !
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	lblActivity.setColour(0, Colours::white);
	*/

	// Used as FOFI... In order to not mess with the visible property
	fifo.fadeOut(&lblActivity, 20);
	fifo.fadeIn(&lblActivity, 20);
}

void FlexBoxItem::textEditorTextChanged(TextEditor &sender)
{
	SessionSingleton::Set(sender.getName().toStdString(), sender.getText().toStdString(), false);
}

// Dedicated to the midi device names
void FlexBoxItem::comboBoxChanged(ComboBox *sender)
{
	// Not here... Since we have to warn also the main ! Circular reference...
	/*
	if ()
	{
	SessionSingleton::releaseMidiDevice()
	}
	*/
	// The session have to know that the status of both old and new devices have changed
	SessionSingleton::Set(sender->getName().toStdString(), sender->getText().toStdString(), true);
	//SessionSingleton::AcquireMidiDevice()
}

void FlexBoxItem::buttonClicked(Button* sender)
{
	if (StringStartsWith(RightDollar(sender->getName().toStdString(), "_"), "chk"))
	{
		// Buttons here are in fact checkboxes
		SessionSingleton::Set(CheckBoxToButton(sender->getName()), (sender->getToggleState() ? "1" : "0"), false);

		if (StringEndsWith(sender->getName().toStdString(), "Solo"))
		{
			// Give access to the underlying FlexBox... Not to our FlexBoxPanel :(
			//m_flexItem.associatedFlexBox->
			if (chkSolo.getToggleState())
			{
				m_parent->OnlyOneSolo(m_panel, m_in);
			}
		}
	}
	else
	{
		if (StringEndsWith(sender->getName().toStdString(), "Copy"))
		{
			Copy();
		}
		else if (StringEndsWith(sender->getName().toStdString(), "Paste"))
		{
			Paste();
		}
		else if (StringEndsWith(sender->getName().toStdString(), "Reset"))
		{
			Reset();
		}
		else // if (... "Clear")
		{
			// Since "this" holds all the controls of the panel...
			// Other buttons are dedicated to their corresponding combo
			cboDevice.setSelectedItemIndex(-1);
		}
	}
}

void FlexBoxItem::DisableSolo()
{
	if (chkSolo.getToggleState())
	{
		chkSolo.setToggleState(false, NotificationType::sendNotification); // Sync or Async...
	}
}

void FlexBoxItem::Copy()
{
	SessionSingleton::setSourcePanel(m_panel);
	SessionSingleton::setSourceIn(m_in);
}

void FlexBoxItem::Paste()
{
	int sourcePanel = SessionSingleton::getSourcePanel();
	bool sourceIn = SessionSingleton::getSourceIn();

	if ((sourcePanel == -1) || ((sourcePanel == m_panel) && (sourceIn == m_in)))
	{
		return;
	}

	std::string prefix = (sourceIn ? "I" : "O") + std::to_string(sourcePanel) + "_";

	/*
	if (sourceIn == m_in)
	{
	cboDevice.set ?
	}
	*/
	txtNibbleA.setText(juce::String(SessionSingleton::Get(prefix + "NibbleA")));
	txtNibbleB.setText(juce::String(SessionSingleton::Get(prefix + "NibbleB")));
	txtNibbleC.setText(juce::String(SessionSingleton::Get(prefix + "NibbleC")));
	txtNibbleD.setText(juce::String(SessionSingleton::Get(prefix + "NibbleD")));
	txtNibbleE.setText(juce::String(SessionSingleton::Get(prefix + "NibbleE")));
	txtNibbleF.setText(juce::String(SessionSingleton::Get(prefix + "NibbleF")));
	txtByteAB.setText(juce::String(SessionSingleton::Get(prefix + "ByteAB")));
	txtByteCD.setText(juce::String(SessionSingleton::Get(prefix + "ByteCD")));
	txtByteEF.setText(juce::String(SessionSingleton::Get(prefix + "ByteEF")));
	chkSplitAB.setToggleState((SessionSingleton::Get(prefix + "SplitAB") == "1"), NotificationType::sendNotification);
	chkSplitCD.setToggleState((SessionSingleton::Get(prefix + "SplitCD") == "1"), NotificationType::sendNotification);
	chkSplitEF.setToggleState((SessionSingleton::Get(prefix + "SplitEF") == "1"), NotificationType::sendNotification);
	chkMute.setToggleState((SessionSingleton::Get(prefix + "Mute") == "1"), NotificationType::sendNotification);
	chkSolo.setToggleState((SessionSingleton::Get(prefix + "Solo") == "1"), NotificationType::sendNotification);
	if (sourceIn && m_in)
	{
		chkSE.setToggleState((SessionSingleton::Get(prefix + "SE") == "1"), NotificationType::sendNotification);
		chkRS.setToggleState((SessionSingleton::Get(prefix + "RS") == "1"), NotificationType::sendNotification);
		chkAT.setToggleState((SessionSingleton::Get(prefix + "AT") == "1"), NotificationType::sendNotification);
		chkRF.setToggleState((SessionSingleton::Get(prefix + "RF") == "1"), NotificationType::sendNotification);
	}
}

void FlexBoxItem::Reset() // AKA cut !-)
{
	cboDevice.setSelectedItemIndex(-1);
	txtNibbleA.setText(""); // clear() does not work...
	txtNibbleB.setText("");
	txtNibbleC.setText("");
	txtNibbleD.setText("");
	txtNibbleE.setText("");
	txtNibbleF.setText("");
	txtByteAB.setText("");
	txtByteCD.setText("");
	txtByteEF.setText("");
	chkSplitAB.setToggleState(false, NotificationType::sendNotification);
	chkSplitCD.setToggleState(false, NotificationType::sendNotification);
	chkSplitEF.setToggleState(false, NotificationType::sendNotification);
	chkMute.setToggleState(false, NotificationType::sendNotification);
	chkSolo.setToggleState(false, NotificationType::sendNotification);
	if (m_in)
	{
		chkSE.setToggleState(false, NotificationType::sendNotification);
		chkRS.setToggleState(false, NotificationType::sendNotification);
		chkAT.setToggleState(false, NotificationType::sendNotification);
		chkRF.setToggleState(false, NotificationType::sendNotification);
	}
}

//TODO : Needed ?
void FlexBoxItem::refreshLayout()
{
	if (auto parent = getParentComponent())
	{
		parent->resized();
	}
}

void FlexBoxItem::paint(Graphics& g)
{
	auto r = getLocalBounds();

	g.setColour(m_colour);
	g.fillRect(r);

	//g.setColour (Colours::black);
	//g.drawFittedText ("w: " + String (r.getWidth()) + newLine + "h: " + String (r.getHeight()), r.reduced (4), Justification::bottomRight, 2);
}

//////////////////
// FlexBoxPanel //
//////////////////

FlexBoxPanel::FlexBoxPanel(int itemCount, int width, int height, int colorIn, int colorOut)
{
	// We don't need it...
	// setupPropertiesPanel();
	setupFlexBoxItems(itemCount, colorIn, colorOut);
	m_flexBox.flexDirection = FlexBox::Direction::column;
	setSize(580, 120 * itemCount);
}

void FlexBoxPanel::resized()
{
	m_flexBox.performLayout(getFlexBoxBounds());
}

juce::Rectangle<float> FlexBoxPanel::getFlexBoxBounds() const
{
	//return getLocalBounds().withTrimmedLeft(300).reduced(10).toFloat();
	return getLocalBounds().reduced(10).toFloat();
}

void FlexBoxPanel::paint(Graphics& g)
{
	g.fillAll(Colours::lightgrey);
	g.setColour(Colours::white);
	g.fillRect(getFlexBoxBounds());
}

void FlexBoxPanel::setupFlexBoxItems(int itemCount, int colorIn, int colorOut)
{
	for (int i = 0; i < itemCount; i++)
	{
		addItem(Colour(colorIn), i, true);
		addItem(Colour(colorOut), i, false);
	}
}

void FlexBoxPanel::addItem(Colour colour, int index, bool in)
{
	m_flexBox.items.add(FlexItem(100, 100).withMargin(10)); // .withWidth(200));

	auto& flexItem = m_flexBox.items.getReference(m_flexBox.items.size() - 1);
	auto panel = m_items.add(new FlexBoxItem(colour, flexItem, index, in, this));
	flexItem.associatedComponent = panel;
	addAndMakeVisible(panel);
}

FlexBox m_flexBox; // Here we have the 1 - N relation
OwnedArray<FlexBoxItem> m_items; // That's why the items have only a reference to a FlexItem instance

void FlexBoxPanel::Blink(int panel, bool in)
{
	m_items[panel * 2 + (in ? 0 : 1)]->DrawActivityLed();
}

void FlexBoxPanel::AvailableDevice(const std::string &name, bool in)
{
	//TODO: ... Use panel and in as parameters ? We must use mml protected method as well !
}

void FlexBoxPanel::UnavailableDevice(const std::string &name, bool in)
{
	//TODO: ... Idem
}

void FlexBoxPanel::OnlyOneSolo(int panel, bool in)
{
	for (int i = 0; i < m_items.size(); i++)
	{
		if (i != panel * 2 + (in ? 0 : 1))
		{
			m_items[i]->DisableSolo();
		}
	}
}
