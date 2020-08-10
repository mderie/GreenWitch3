/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.hpp"
#include "exprtk.hpp"
#include "ConfigurationFiles.hpp"
#include "CommonStuffs.hpp"

#include <string>

//==============================================================================

///////////////////////////////////////////////////////////////////////////////////////
// Strange beast (break encapsulation rule among others, but it compiles...)         //
// Probably not the most elegant solution. But at least there are no circular ref :) //
///////////////////////////////////////////////////////////////////////////////////////

MainComponent* mainWindow = nullptr;

//TODO: How the SessionData or the FlexStuff should call LogToScreen ? Create a new couple header / source for this purpose ?

void OnMidiDeviceStatusChange(const std::string &name, DeviceStatus value)
{
	mainWindow->MidiDeviceStatusChange(name, value); // Issue "solved" here : we have to propagate the event into the class sole instance
}

// Provide here the previous status ?
void MainComponent::MidiDeviceStatusChange(const std::string &name, DeviceStatus value)
{
	if (value == DeviceStatus::dsFree)
	{
		CloseMidiDevice(name.substr(1), (name[0] == 'I'));
		LogToScreen(name + " ==> Device closed");
	}
	else if (value == DeviceStatus::dsBusyInternal)
	{
		OpenMidiDevice(name.substr(1), (name[0] == 'I'));
		LogToScreen(name + " ==> Device opened");
	}
	//TODO: We will see later for the dsBusyExternal !
}

/////////////////
// ctor & dtor //
/////////////////

MainComponent::MainComponent()
{
	m_appBaseFolder = File::addTrailingSeparator(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()).toStdString();
	SetLogFolder(m_appBaseFolder);

	ReadConfiguration();
	LogThis("MainComponent::MainComponent ==> Session start", Target::misc);

	mainWindow = this;
	SessionSingleton::RegisterEventHandler(&OnMidiDeviceStatusChange);

	//SessionSingleton::RegisterMidiDevice(SessionSingleton::NO_DEVICE, true);
	int index = 0;
	for (const auto &item : juce::MidiInput::getDevices())
	{
		LogThis2("MainComponent::MainComponent ==> Found input midi device called '%s'", Target::midiIn, item.toStdString().c_str());
		SessionSingleton::RegisterMidiDevice(item.toStdString(), true);
		m_mds.push_back(MidiDevice(index++, item.toStdString(), true)); // Argh, it smells the double data structure...
	}

	//SessionSingleton::RegisterMidiDevice(SessionSingleton::NO_DEVICE, false);
	index = 0;
	for (const auto &item : juce::MidiOutput::getDevices())
	{
		LogThis2("MainComponent::MainComponent ==> Found output midi device called '%s'", Target::midiOut, item.toStdString().c_str());
		SessionSingleton::RegisterMidiDevice(item.toStdString(), false);
		m_mds.push_back(MidiDevice(index++, item.toStdString(), false));
	}

	setSize(800, 600);

	logArea.setReadOnly(true);
	logArea.setMultiLine(true);
	//logArea.setBounds(0, 400, 800, 200);
	logArea.setName("logArea");
	addAndMakeVisible(logArea);

	RestoreLastSession();

	boxPanel = new FlexBoxPanel(m_panelCount, getWidth() - 12, (m_panelCount * 300) - 12, m_colorIn, m_colorOut);
	//panel->setName("panel");
	//panel->setBounds(0, 0, 800, 400);

	//scrollArea.setBounds(0, 0, 800, 400);
	scrollArea.setViewedComponent(boxPanel);
	addAndMakeVisible(scrollArea);
	
	m_mmd.startThread(); // Explicit !-)

	resized();
	LogToScreen("Ready !-)");
}

MainComponent::~MainComponent()
{
	LogThis("MainComponent::~MainComponent ==> Session stop", Target::misc);

	m_mmd.stopThread(500);

	BackupCurrentSession();
	CloseAllMidiDevices();

	//delete panel;
}

////////////////////
// Business logic //
////////////////////

int MainComponent::FindMidiDevice(const std::string &name, bool in)
{	
	for (int i = 0; i < m_mds.size(); i++)
	{
		if ((m_mds[i].Name == name) && (m_mds[i].In == in))
		{
			// Given a vector<int> v; vector<int>::iterator it; We could have written
			// return std::distance(v.begin(), it);
			return i;
		}
	}

	return -1;
}

// Needed here ? Better in the session ?
std::vector<std::string> MainComponent::DumpMidiDeviceNames(bool in)
{
	std::vector<std::string> results;

	for (const auto &item : m_mds)
	{
		if (item.In == in)
		{
			results.push_back(item.Name);
		}
	}

	return results;
}

void MainComponent::LogToScreen(const std::string &what) //TODO: Find a better name, for the area as well
{
	logArea.moveCaretToEnd();
	logArea.insertTextAtCaret(juce::String(what + "\n"));
	//Needed ? logArea.scrollToMakeSureCursorIsVisible();

	//TODO: Append also into a file ?
}

void MainComponent::ReadConfiguration()
{
	//String filePath = File::getCurrentWorkingDirectory().getFullPathName(); // Review this... Done :)
	File executableFileName = File::getSpecialLocation(File::currentExecutableFile);
	//File executableFolder = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory();
	//std::string fullFileName = FindAndReplace(s, ".exe", ".ini"); // const_cast<> & Co works only with pointer
	std::string fullFileName = m_appBaseFolder + executableFileName.getFileNameWithoutExtension().toStdString() + ".ini";

	ConfigurationFile cf(fullFileName, false);

	//SetLogFilter((cf.Read("Log", "screen", "0") == "1"), Target::screen);
	SetLogFilter((cf.Read("Log", "midiIn", "0") == "1"), Target::midiIn);
	SetLogFilter((cf.Read("Log", "midiOut", "0") == "1"), Target::midiOut);
	SetLogFilter((cf.Read("Log", "misc", "0") == "1"), Target::misc);

	m_sessionAutoSave = (cf.Read("Session", "autoSave", "1") == "1");
	m_sessionFileName = cf.Read("Session", "fileName", "session.ini");

	m_panelCount = CSTOUL(cf.Read("UI", "panelCount", "10"), 10);
	if (m_panelCount == 0)
	{
		m_panelCount = 10;
	}

	m_logPanelHeight = CSTOUL(cf.Read("UI", "logPanelHeight", "100"), 100);
	if (m_logPanelHeight == 0)
	{
		m_logPanelHeight = 100;
	}

	m_colorIn = CSTOUL(cf.Read("UI", "colorIn", "4278231784"), 4278231784);
	if (m_colorIn == 0)
	{
		m_colorIn = 4278231784;
	}

	m_colorOut = CSTOUL(cf.Read("UI", "colorOut", "4294934311"), 4294934311);
	if (m_colorOut == 0)
	{
		m_colorOut = 4294934311;
	}

	// Create the file for the next time :)
	if (!FileExists(fullFileName))
	{
		cf.Write("Log", "screen", "0");
		cf.Write("Log", "midiIn", "0");
		cf.Write("Log", "midiOut", "0");
		cf.Write("Log", "misc", "0");
		cf.Write("Session", "fileName", "session.ini");
		cf.Write("Session", "autoSave", "1");
		cf.Write("UI", "panelCount", "10");
		cf.Write("UI", "logPanelHeight", "100");
		cf.Write("UI", "colorlIn", "4278231784");
		cf.Write("UI", "colorlOut", "4294934311");
	}
}

void MainComponent::RestoreLastSession()
{
	std::string fullFileName = m_appBaseFolder + m_sessionFileName;
	if (FileExists(fullFileName))
	{
		SessionSingleton::Deserialize(fullFileName);
		SessionSingleton::RemoveMissingMidiDevice();
		SessionSingleton::RemoveExtraPanel(m_panelCount);
		SessionSingleton::RemoveExtraSolo();
	}
}

void MainComponent::BackupCurrentSession()
{
	if (m_sessionAutoSave && SessionSingleton::Changed())
	{
		std::string fullFileName = m_appBaseFolder + m_sessionFileName;
		SessionSingleton::Serialize(fullFileName);
	}
}

// Called at start time and at each combo opening time
void MainComponent::RefreshMidiDeviceStatus(bool in)
{
	//TODO: ...
}

std::string MainComponent::DumpMidiMessage(const juce::MidiMessage &mm)
{
	std::stringstream ss;
	const juce::uint8* p = mm.getRawData();

	for (int i = 0; i < mm.getRawDataSize(); i++)
	{
		ss << "0x" << std::hex << static_cast<int>(*(p + i)) << " ";
	}

	return ss.str();
}

void MainComponent::OpenMidiDevice(const std::string &name, bool in)
{
	LogThis2("MainComponent::OpenMidiDevice ==> Entering, device = '%s'", (in ? Target::midiIn : Target::midiOut), name.c_str(), in);

	if (name == "")
	{
		LogThis("MainComponent::OpenMidiDevice ==> Leaving, no device", (in ? Target::midiIn : Target::midiOut));
		return;
	}

	int id = FindMidiDevice(name, in);
	if (id == -1) // Not sure if this case could happened...
	{
		// Can't craft a string directly, since none of the operands are strings...
		// LogToScreen("Unable to open " + (in ? "in" : "out") + " midi device named " + name);
		// https://stackoverflow.com/questions/20449590/how-can-i-combine-multiple-chars-to-make-a-string
		std::stringstream ss; // decl + init must be in two lines https://stackoverflow.com/questions/21924156/how-to-initialize-a-stdstringstream
		ss << "Unable to open " << (in ? "in" : "out") << " midi device named " << name;
		LogToScreen(ss.str());
		LogThis("MainComponent::OpenMidiDevice ==> Leaving, bad device id", (in ? Target::midiIn : Target::midiOut));
		return;
	}

	if (in)
	{
		LogThis2("MainComponent::OpenMidiDevice ==> Starting device '%s'", (in ? Target::midiIn : Target::midiOut), name.c_str());
		m_mds[id].MI = juce::MidiInput::openDevice(m_mds[id].Index, this);
		if (m_mds[id].MI)
		{
			m_mds[id].MI->start();
		}
		else
		{
			LogToScreen("Unable to open midi in device");
			LogThis("MainComponent::OpenMidiDevice ==> Leaving, Unable to open midi device", (in ? Target::midiIn : Target::midiOut));
			return;
		}
	}
	else 
	{
		LogThis2("MainComponent::OpenMidiDevice ==> Starting device 's%'", (in ? Target::midiIn : Target::midiOut), name.c_str());
		m_mds[id].MO = juce::MidiOutput::openDevice(m_mds[id].Index);
		if (m_mds[id].MO)
		{
			LogThis2("MainComponent::OpenMidiDevice ==> Device started", (in ? Target::midiIn : Target::midiOut));
		}
		else
		{
			LogToScreen("Unable to open midi out device");
			LogThis("MainComponent::OpenMidiDevice ==> Leaving, Unable to open midi device", (in ? Target::midiIn : Target::midiOut));
			return;
		}
	}
	LogThis2("MainComponent::OpenMidiDevice ==> Leaving", (in ? Target::midiIn : Target::midiOut));
}

void MainComponent::CloseMidiDevice(const std::string &name, bool in)
{
	LogThis2("MainComponent::CloseMidiDevice ==> Entering, device = '%s'", (in ? Target::midiIn : Target::midiOut) , name.c_str(), in);

	int id = FindMidiDevice(name, in);
	if (id == -1)
	{
		LogThis("MainComponent::CloseMidiDevice ==> Leaving, bad device id", (in ? Target::midiIn : Target::midiOut));
		return;
	}

	CloseMidiDevice(id);
	LogThis2("MainComponent::CloseMidiDevice ==> Leaving", (in ? Target::midiIn : Target::midiOut));
}

void MainComponent::CloseMidiDevice(int id)
{
	if (m_mds[id].In && m_mds[id].MI)
	{
		try
		{
			LogThis2("MainComponent::CloseMidiDevice ==> closing device = '%s'", Target::midiIn, m_mds[id].Name.c_str());
			m_mds[id].MI->stop();
			delete m_mds[id].MI;
			m_mds[id].MI = nullptr;
		}		
		catch (...)
		{
			LogToScreen("Unable to close midi in device");
		}
	}
	else if (!m_mds[id].In && m_mds[id].MO)
	{
		try
		{
			LogThis2("MainComponent::CloseMidiDevice ==> closing device = '%s'", Target::midiOut, m_mds[id].Name.c_str());
			delete m_mds[id].MO;
			m_mds[id].MO = nullptr;
		}
		catch (...)
		{
			LogToScreen("Unable to close midi out device");
		}
	}
}

void MainComponent::CloseAllMidiDevices()
{
	for (int i = 0; i < m_mds.size(); i++)
	{
		CloseMidiDevice(i);
	}
}

/////////////////////////////////////////////////////////////////
// Midi Message Manipulation (should be moved in another unit) //
/////////////////////////////////////////////////////////////////

bool MainComponent::Compare(const std::string &filterValue, byte currentValue)
{
	byte value = (byte) CSTOI(filterValue.substr(1)); //TODO: We 'll see later for hex support

	if (filterValue[0] == '!')
	{
		return currentValue != value;
	}
	else if (filterValue[0] == '<')
	{
		return currentValue < value;
	}
	else if (filterValue[0] == '>')
	{
		return (currentValue > value);
	}
	else if (filterValue[0] == '=')
	{
		return (currentValue == value);
	}
	else
	{
		LogThis2("MainComponent::Compare ==> Unknown first character = %c", Target::misc, filterValue[0]);
		return false;
	}
}

bool MainComponent::Filter(int panel, const NibbleReadyMidiMessage &mm)
{
	std::string value;

	//TODO: Convert back controlContents[(int) ControlContent::SplitAB] to plain old #define ... usage ?

	// AB

	if (SessionSingleton::FindControlByContent(panel, ControlContent::SplitAB, true) == "1")
	{
		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleA, true));
		if ((value.size() > 1) and (!Compare(value, mm.NibbleA)))
		{
			return false;
		}

		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleB, true));
		if ((value.size() > 1) and (!Compare(value, mm.NibbleB)))
		{
			return false;
		}
	}
	else
	{
		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::ByteAB, true));
		if ((value.size() > 1) and (!Compare(value, mm.GetAB())))
		{
			return false;
		}
	}

	// CD

	if (SessionSingleton::FindControlByContent(panel, ControlContent::SplitCD, true) == "1")
	{
		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleC, true));
		if ((value.size() > 1) and (!Compare(value, mm.NibbleC)))
		{
			return false;
		}

		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleD, true));
		if ((value.size() > 1) and (!Compare(value, mm.NibbleD)))
		{
			return false;
		}
	}
	else
	{
		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::ByteCD, true));
		if ((value.size() > 1) and (!Compare(value, mm.GetCD())))
		{
			return false;
		}
	}

	// EF

	if (SessionSingleton::FindControlByContent(panel, ControlContent::SplitEF, true) == "1")
	{
		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleE, true));
		if ((value.size() > 1) and (!Compare(value, mm.NibbleE)))
		{
			return false;
		}

		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleF, true));
		if ((value.size() > 1) and (!Compare(value, mm.NibbleF)))
		{
			return false;
		}
	}
	else
	{
		value = StringTrim(SessionSingleton::FindControlByContent(panel, ControlContent::ByteEF, true));
		if ((value.size() > 1) and (!Compare(value, mm.GetEF())))
		{
			return false;
		}
	}

	return true;
}

void MainComponent::Modify(int panel, NibbleReadyMidiMessage &mm)
{
	typedef exprtk::symbol_table<double> symbol_table_t;
	typedef exprtk::expression<double> expression_t;
	typedef exprtk::parser<double> parser_t;

	symbol_table_t symbol_table;
	expression_t expression;
	parser_t parser;

	// The lib does not work well with byte only algebra... 
	// Care: had to add /bigobj to the project C/C++ compile option at the command line !
	// All the time Projucer regenerate the project / solution files (care of the new _App stuff)
	double a, b, ab, c, d, cd, e, f, ef;

	a = mm.NibbleA;
	b = mm.NibbleB;
	ab = mm.GetAB();
	c = mm.NibbleC;
	d = mm.NibbleD;
	cd = mm.GetCD();
	e = mm.NibbleE;
	f = mm.NibbleF;
	ef = mm.GetEF();

	symbol_table.add_variable("A", a);
	symbol_table.add_variable("B", b);
	symbol_table.add_variable("AB", ab);
	symbol_table.add_variable("C", c);
	symbol_table.add_variable("D", d);
	symbol_table.add_variable("CD", cd);
	symbol_table.add_variable("E", e);
	symbol_table.add_variable("F", f);
	symbol_table.add_variable("EF", ef);
	expression.register_symbol_table(symbol_table);

	std::string formula;

	// AB

	if (SessionSingleton::FindControlByContent(panel, ControlContent::SplitAB, false) == "1")
	{
		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleA, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.NibbleA = static_cast<byte>(expression.value());
		}

		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleB, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.NibbleB = static_cast<byte>(expression.value());
		}
	}
	else
	{
		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::ByteAB, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.SetAB(static_cast<byte>(expression.value()));
		}
	}
	
	// CD

	if (SessionSingleton::FindControlByContent(panel, ControlContent::SplitCD, false) == "1")
	{
		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleC, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.NibbleC = static_cast<byte>(expression.value());
		}

		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleD, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.NibbleD = static_cast<byte>(expression.value());
		}
	}
	else
	{
		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::ByteCD, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.SetCD(static_cast<byte>(expression.value()));
		}
	}

	// EF

	if (SessionSingleton::FindControlByContent(panel, ControlContent::SplitEF, false) == "1")
	{
		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleE, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.NibbleE = static_cast<byte>(expression.value());
		}

		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::NibbleF, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.NibbleF = static_cast<byte>(expression.value());
		}
	}
	else
	{
		formula = StringUpper(SessionSingleton::FindControlByContent(panel, ControlContent::ByteEF, false));
		if (formula.size() > 0 && (parser.compile(formula, expression)))
		{
			mm.SetEF(static_cast<byte>(expression.value()));
		}
	}
}

void MainComponent::BroadCast(int panel, const NibbleReadyMidiMessage &mm)
{
	// - Send the message

	juce::MidiMessage msg(mm.GetAB(), mm.GetCD(), mm.GetEF());
	std::string name = SessionSingleton::FindControlByContent(panel, ControlContent::Device, false);
	int id = FindMidiDevice(name, false);
	if (id == -1)
	{
		LogThis("MainComponent::BroadCast ==> Leaving, bad device id", Target::midiOut);
		return;
	}
	LogThis2("MainComponent::BroadCast ==> Message to device '%s' : '%s'", Target::midiOut, name.c_str(), DumpMidiMessage(msg).c_str());
	if (m_mds[id].MO)
	{
		m_mds[id].MO->sendMessageNow(msg);
	}

	// Notify the UI through the activity led
	boxPanel->Blink(panel, false);
}

void MainComponent::Process(int panel, NibbleReadyMidiMessage &mm)
{
	bool filter = Filter(panel, mm);
	if (filter or(!filter and(SessionSingleton::FindControlByContent(panel, ControlContent::RF, true) == "1")))
	{
		// And here we are at the output side
		Modify(panel, mm);
		LogThis2("MainComponent::Process ==> Modified message about to be sent : %s", Target::midiOut, mm.ToString().c_str());
		BroadCast(panel, mm);
	}
	else
	{
		LogThis2("MainComponent::Process ==> Message didn't pass the filter", Target::midiIn);
	}
}

////////////////////////////////////////////
// Event handlers from the "audio" thread //
////////////////////////////////////////////

void MainComponent::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
	LogThis2("MainComponent::handleIncomingMidiMessage ==> Message from device '%s' : '%s'", Target::midiIn, source->getName().toStdString().c_str(), DumpMidiMessage(message).c_str());

	// Iterate on the source(s)
	for (const auto &panel : SessionSingleton::FindPanelByDevice(source->getName().toStdString(), true))
	{
		// Notify the UI through the activity led
		boxPanel->Blink(panel, true);

		if (SessionSingleton::FindControlByContent(panel, ControlContent::Mute, true) == "1")
		{
			LogThis2("MainComponent::handleIncomingMidiMessage ==> midi device in muted", Target::midiIn);
			continue;
		}

		if (SessionSingleton::FindControlByContent(panel, ControlContent::Device, false) == "")
		{
			LogThis2("MainComponent::handleIncomingMidiMessage ==> No corresponding midi device out", Target::midiIn);
			continue;
		}

		if (SessionSingleton::FindControlByContent(panel, ControlContent::Mute, false) == "1")
		{
			LogThis2("MainComponent::handleIncomingMidiMessage ==> midi device out muted", Target::midiOut);
			continue;
		}

		std::string soloDevice = SessionSingleton::FindSoloDevice(true);
		if ((soloDevice != "") and (soloDevice != source->getName().toStdString()))
		{
			LogThis2("MainComponent::handleIncomingMidiMessage ==> solo device in is not me", Target::midiIn);
			continue;
		}

		soloDevice = SessionSingleton::FindSoloDevice(false);
		if ((soloDevice != "") and (soloDevice != SessionSingleton::FindDeviceByPanel(panel, false)))
		{
			LogThis2("MainComponent::handleIncomingMidiMessage ==> solo device out is not me", Target::midiOut);
			continue;
		}

		if (message.getRawDataSize() == 3)
		{
			// It is a standard midi message
			const juce::uint8* p = message.getRawData();
			NibbleReadyMidiMessage mm(*(p + 0), *(p + 1), *(p + 2));
			Process(panel, mm);
		}
		else
		{
			int id = FindMidiDevice(source->getName().toStdString(), true);
			if (id == -1)
			{
				LogThis("MainComponent::handleIncomingMidiMessage ==> Leaving, bad device id", Target::midiOut);
				continue;
			}
						
			if (message.getRawDataSize() > 3)
			{
				//TODO : Add if first byte = ... ?
				LogThis("MainComponent::handleIncomingMidiMessage ==> Sysex passthrough ?", Target::midiOut);
				if (SessionSingleton::FindControlByContent(panel, ControlContent::SE, true) == "1")
				{
					if (m_mds[id].MO)
					{
						LogThis2("MainComponent::handleIncomingMidiMessage ==> Message to device '%s' : '%s'", Target::midiOut, m_mds[id].Name.c_str(), DumpMidiMessage(message).c_str());
						m_mds[id].MO->sendMessageNow(message);
					}
				}
			}
			else
			{
				//TODO: Add if first byte = ... ?
				LogThis("MainComponent::handleIncomingMidiMessage ==> AfterTouch passthrough ?", Target::midiOut);
				if (SessionSingleton::FindControlByContent(panel, ControlContent::AT, true) == "1")
				{
					if (m_mds[id].MO)
					{
						LogThis2("MainComponent::handleIncomingMidiMessage ==> Message to device '%s' : '%s'", Target::midiOut, m_mds[id].Name.c_str(), DumpMidiMessage(message).c_str());
						m_mds[id].MO->sendMessageNow(message);
					}
				}
			}

			//TODO: we should also store the last RS before using it here
			//m_mds[id].LastRunningStatus
			//
			//NibbleReadyMidiMessage mm(*(p + 0), *(p + 1), *(p + 2));
			//Process(panel, mm);

			// https://forum.juce.com/t/midi-running-status/8387/3 ==> Done automagically !
		}
	}
}

///////////////////////////////////////
// Event handlers from the UI thread //
///////////////////////////////////////

void MainComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
	/*
    g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setFont(Font (16.0f));
    g.setColour(Colours::white);
    g.drawText("Hello World!", getLocalBounds(), Justification::centred, true);
	*/
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

	scrollArea.setBounds(0, 0, getWidth(), getHeight() - m_logPanelHeight);
	/*
	if (boxPanel)
	{
		boxPanel->setBounds(0, 0, getWidth() - 12, getHeight() - 200 - 12);
		//boxPanel->resized();
	}
	*/
	
	logArea.setBounds(0, getHeight() - m_logPanelHeight, getWidth(), m_logPanelHeight);
}

// See also FlexItem & Co... In the DemoRunner -> FlexBoxDemo