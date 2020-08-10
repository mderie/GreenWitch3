/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FlexComponent.hpp"
#include "SessionData.hpp"
#include "MidiDeviceData.hpp"

#include <map>

typedef unsigned char byte; // Same as juce::uint8

struct MidiDevice
{
	int Index;
	std::string Name;
	bool In;
	byte LastRunningStatus;
	// panel pointers or indices ?
	DeviceStatus DS; // Needed for the UI Feedback
	bool Activity; // Idem
	MidiInput *MI;
	MidiOutput *MO;
	MidiDevice(int index, const std::string &name, bool in)
	{
		Index = index;
		Name = name;
		In = in;
		LastRunningStatus = 0;
		DS = DeviceStatus::dsFree;
		MI = nullptr;
		MO = nullptr;
	}
};

// Needed ? RS is always two bytes ? Same goes for AT ? Care : SE can be longer...
struct MidiMessageModifier
{
	bool Mute, Solo, FilterSystemExclusive, FilterRunningStatus, FilterAfterTouch;
	// pointer to MidiDevice ?
	// ABCDEF formula / filter pointers ?
	// panel pointers or indices ?
};

//TODO: Move this to a new unit ? IE : the unused yet MidiData !
struct NibbleReadyMidiMessage
{
	byte NibbleA, NibbleB, NibbleC, NibbleD, NibbleE, NibbleF;

	// Needed ?
	NibbleReadyMidiMessage()
	{
		NibbleA = NibbleB = NibbleC = NibbleD = NibbleE = NibbleF = 0;
	}

	NibbleReadyMidiMessage(byte ab, byte cd, byte ef)
	{
		SetAB(ab);
		SetCD(cd);
		SetEF(ef);
	}
	inline byte GetAB() const { return (NibbleA << 4) + NibbleB; }
	inline void SetAB(byte value) { NibbleA = value >> 4; NibbleB = value & 0x0F; }

	inline byte GetCD() const { return (NibbleC << 4) + NibbleD; }
	inline void SetCD(byte value) { NibbleC = value >> 4; NibbleD = value & 0x0F; }

	inline byte GetEF() const { return (NibbleE << 4) + NibbleF; }
	inline void SetEF(byte value) { NibbleE = value >> 4; NibbleF = value & 0x0F; }

	std::string ToString()
	{
		std::stringstream ss;
		ss << "0x" << std::hex << (int) NibbleA << (int) NibbleB << " 0x" << (int) NibbleC << (int) NibbleD << " 0x" << (int) NibbleE << (int) NibbleF; // Long line he ?-)
		return ss.str();
	} 
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

// We should not inherit from MidiInputCallback here since it forces the business logic (code & date) related to the handler to rely in the main component...
class MainComponent : public Component, public juce::MidiInputCallback
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
	virtual void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
	/*
	virtual void comboBoxChanged(ComboBox *sender) override;
	virtual void textEditorTextChanged(TextEditor &sender) override; // Oddly enough it is not *sender...
	*/
	void MidiDeviceStatusChange(const std::string &name, DeviceStatus value);

private:
    //==============================================================================
    // Your private member variables go here...
	FlexBoxPanel *boxPanel = nullptr; // Provides getter based on index ? Nope, not needed !-)
	TextEditor logArea;
	Viewport scrollArea;

	MonitorMidiDevices m_mmd;

	//juce::StringArray m_midiInputDeviceNames;
	//juce::StringArray m_midiOutputDeviceNames;

	std::vector<MidiDevice> m_mds;

	unsigned int m_panelCount;
	unsigned int m_logPanelHeight;
	unsigned int m_colorIn;
	unsigned int m_colorOut;

	bool m_sessionChanged;
	bool m_sessionAutoSave; //TODO: When false we need then an explicit load & save...

	std::string m_sessionFileName;
	std::string m_appBaseFolder;

	std::string DumpMidiMessage(const juce::MidiMessage &mm);
	std::vector<std::string> DumpMidiDeviceNames(bool in);

	void LogToScreen(const std::string &what);
	void ReadConfiguration();
	void RestoreLastSession();
	void BackupCurrentSession();
	void OpenMidiDevice(const std::string &name, bool in);
	void CloseAllMidiDevices();
	void CloseMidiDevice(int id);
	void CloseMidiDevice(const std::string &name, bool in);
	void RefreshMidiDeviceStatus(bool in);
	int FindMidiDevice(const std::string &name, bool in);

	// The midi message stays on the stack all the time (cheap multithreading support :)
	void Process(int panel, NibbleReadyMidiMessage &mm);
	bool Compare(const std::string &filterValue, byte currentValue);
	bool Filter(int panel, const NibbleReadyMidiMessage &mm);
	void Modify(int panel, NibbleReadyMidiMessage &mm);
	void BroadCast(int panel, const NibbleReadyMidiMessage &mm);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
