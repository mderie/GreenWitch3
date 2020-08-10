
#include "SessionData.hpp"
#include "CommonStuffs.hpp"
#include "ConfigurationFiles.hpp" // Should be renamed iniFile

//#include <regex>
#include <algorithm>

// Call the static ctor's
std::map<std::string, std::string> SessionSingleton::s_contents; // Use a typedef ?
std::map<std::string, std::vector<int> > SessionSingleton::s_deviceUsages; // Hold for each device the panel indexes where it appears, could be none
int SessionSingleton::s_sourcePanel = -1;
bool SessionSingleton::s_changed = false;
bool SessionSingleton::s_sourceIn = false;
MidiDeviceStatusChange SessionSingleton::s_callback = nullptr; // Weird syntax : for static init, we have to repeat the type
//const std::string SessionSingleton::NO_DEVICE = "<No midi device>";

// Control names in FlexStuff have a pattern as follow : [I|O][n]_[name]
// Where n in decimal is the zero based panel index
// Midi device names is just [I|O][name]

// Last Session may reference more devices than actually present
void SessionSingleton::RemoveMissingMidiDevice()
{
	bool found = false;

	//for (const auto &kvp : s_contents)
	for (auto it = s_contents.cbegin(); it != s_contents.cend();)	
	{
		if (RightDollar(it->first, "_") == "Device")
		{
			if ((s_deviceUsages.find(it->first[0] + it->second) == s_deviceUsages.end()))
			{
				// This alter the session changed flag...
				//Set(kvp.first, "", false);
				s_contents.erase(it++);
				found = true;
			}			
		}

		if (found)
		{
			found = false;
		}
		else
		{
			++it;
		}
	}
}

// Last Session may reference more panels
void SessionSingleton::RemoveExtraPanel(int panelCount)
{
	//for (auto &kvp : s_contents)
	for (auto it = s_contents.cbegin(); it != s_contents.cend();)
	{
		if (std::stoi(LeftDollar(it->first.substr(1), "_")) >= panelCount)
		{
			s_contents.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void SessionSingleton::RemoveExtraSolo()
{
	int iIndex = -1;
	int oIndex = -1;
	bool found = false;
		
	// Care : erase invalidated the iterator !
	//for (auto &kvp : s_contents)
	// https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
	for (auto it = s_contents.cbegin(); it != s_contents.cend();)
	{
		int panel = std::stoi(LeftDollar(it->first.substr(1), "_"));
		if (StringEndsWith(it->first, "_Solo"))
		{
			if ((it->first[0] == 'I') and (it->second == "1"))
			{
				if (iIndex == -1)
				{
					iIndex = panel;
				}
				else
				{
					s_contents.erase(it++);
					found = true;
				}
			}
			else if ((it->first[0] == 'O') and(it->second == "1"))
			{
				if (oIndex == -1)
				{
					oIndex = panel;
				}
				else
				{
					s_contents.erase(it++);
					found = true;
				}
			}
		}

		if (found)
		{
			found = false;
		}
		else
		{
			++it;
		}
	}
}

void SessionSingleton::RegisterEventHandler(MidiDeviceStatusChange callback)
{
	s_callback = callback;
}

void SessionSingleton::Serialize(const std::string &fullFileName)
{
	ConfigurationFile cf(fullFileName, true); // Normally this will recreate the file

	for (const auto &kvp : s_contents)
	{
		if (kvp.second != "")
		{
			cf.Write("Singleton", kvp.first, kvp.second);
		}
	}

	s_changed = false;
}

void SessionSingleton::Deserialize(const std::string &fullFileName)
{
	ConfigurationFile cf(fullFileName, false); // Just read the file

	std::vector<std::string> keys = cf.ReadKeys("Singleton");
	for (const auto &key : keys)
	{
		if (StringEndsWith(key, "Device"))
		{
			Set(key, cf.Read("Singleton", key, ""), true); // We have to be notified
		}
		else
		{
			s_contents[key] = cf.Read("Singleton", key, "");
		}		
	}

	s_changed = false;
}

/*
std::vector<std::string> SessionSingleton::RetrieveKeys(const std::string &value)
{
	std::vector<std::string> results;

	for (const auto &kvp : s_contents)
	{
		if (s_contents[kvp.second] == value)
		{
			results.push_back(kvp.first);
		}
	}

	return results;
}
*/

std::string SessionSingleton::Get(const std::string &key)
{
	if (s_contents.find(key) == s_contents.end())
	{
		return "";
	}
	else
	{
		return s_contents[key];
	}	
}

void SessionSingleton::Set(const std::string &key, const std::string &value, bool propagate)
{
	// Real change ?
	if ((s_contents.find(key) != s_contents.end()) && (s_contents[key] == value))
	{
		return;
	}

	if (propagate)
	{
		int panel = std::stoi(LeftDollar(key.substr(1), "_"));
		if (s_contents.find(key) != s_contents.end())
		{
			ReleaseMidiDevice(panel, s_contents[key], (key[0] == 'I'));
		}
		AcquireMidiDevice(panel, value, (key[0] == 'I'));
	}

	s_contents[key] = value;
	s_changed = true;
}

std::vector<int> SessionSingleton::FindPanelByDevice(const std::string &name, bool in)
{
	// We could have scan each time the values using a regexp...
	/*	
	std::regex re;
	*/

	std::string midiDeviceName =  (in ? "I" : "O") + name;

	for (const auto &kvp : s_deviceUsages)
	{
		if (kvp.first == midiDeviceName)
		{
			return kvp.second; // Not an optimal return type...
		}
	}

	LogThis2("SessionSingleton::FindPanelByDevice ==> Oups ! in = %d and name = '%s'", (in ? Target::midiIn : Target::midiOut), in, name.c_str());
	return std::vector<int>();
}

std::string SessionSingleton::FindDeviceByPanel(int panel, bool in)
{
	return s_contents[(in ? "I" : "O") + std::to_string(panel) + "_Device"];
}

std::string SessionSingleton::FindControlByContent(int panel, ControlContent cc, bool in)
{
	return s_contents[((in ? "I" : "O") + std::to_string(panel) + "_" + controlContents[(int) cc])];
}

void SessionSingleton::RegisterMidiDevice(const std::string &name, bool in)
{
	std::string midiDeviceName = (in ? "I" : "O") + name;

	s_deviceUsages[midiDeviceName] = std::vector<int>();
}

std::vector<std::string> SessionSingleton::RetrieveMidiDevices(bool in)
{
	std::vector<std::string> results;

	for (const auto &kvp : s_deviceUsages)
	{
		if ((in && (kvp.first[0] == 'I')) || (!in && (kvp.first[0] == 'O')))
		{
			results.push_back(kvp.first.substr(1));
		}
	}

	return results;
}

// Return the device that's want to play solo, an empty string otherwise
std::string SessionSingleton::FindSoloDevice(bool in)
{
	for (auto &kvp : s_contents)
	{
		if ((in and (kvp.first[0] == 'I') and (RightDollar(kvp.first, "_") == "Solo") and (kvp.second == "1")) ||
			(!in and(kvp.first[0] == 'O') and (RightDollar(kvp.first, "_") == "Solo") and (kvp.second == "1")))
		{
			return s_contents[LeftDollar(kvp.first, "_") + "_Device"];
		}
	}

	return "";
}

void SessionSingleton::AcquireMidiDevice(int panel, const std::string &name, bool in)
{
	if (name == "")
	{
		LogThis2("SessionSingleton::AcquireMidiDevice ==> Empty name not allowed", (in ? Target::midiIn : Target::midiOut));
		return;
	}

	std::string midiDeviceName = (in ? "I" : "O") + name;

	if (s_deviceUsages.find(midiDeviceName) == s_deviceUsages.end())
	{
		LogThis2("SessionSingleton::AcquireMidiDevice ==> Device '%s' not present", (in ? Target::midiIn : Target::midiOut), name.c_str());
		return;
	}

	LogThis2("SessionSingleton::AcquireMidiDevice ==> panel = %d and name = '%s'", (in ? Target::midiIn : Target::midiOut), panel, name.c_str());
	
	if ((s_deviceUsages[midiDeviceName].size() == 0) && (s_callback != nullptr))
	{
		LogThis2("SessionSingleton::AcquireMidiDevice ==> about to callback", (in ? Target::midiIn : Target::midiOut));
		s_callback(midiDeviceName, dsBusyInternal);
		LogThis2("SessionSingleton::AcquireMidiDevice ==> done", (in ? Target::midiIn : Target::midiOut));
	}
	s_deviceUsages[midiDeviceName].push_back(panel);
}

void SessionSingleton::ReleaseMidiDevice(int panel, const std::string &name, bool in)
{
	if (name == "")
	{
		return;
	}

	LogThis2("SessionSingleton::ReleaseMidiDevice ==> panel = %d and name = '%s'", (in ? Target::midiIn : Target::midiOut), panel, name.c_str());
	std::string midiDeviceName = (in ? "I" : "O") + name;

	// Does not work, it only returns an iterator
	// std::remove(s_deviceUsages[midiDeviceName].begin(), s_deviceUsages[midiDeviceName].end(), panel);
	// Ouch ! Long line to just remove one element from a container

	// Remove the panel from the device usages
	auto iter = std::find(s_deviceUsages[midiDeviceName].begin(), s_deviceUsages[midiDeviceName].end(), panel);
	if (iter != s_deviceUsages[midiDeviceName].end())
	{
		s_deviceUsages[midiDeviceName].erase(iter);
	}

	// Notify if the device is no more used
	if ((s_deviceUsages[midiDeviceName].size() == 0) && (s_callback != nullptr))
	{
		LogThis("SessionSingleton::ReleaseMidiDevice ==> about to callback", (in ? Target::midiIn : Target::midiOut));
		s_callback(midiDeviceName, dsFree);
		LogThis("SessionSingleton::ReleaseMidiDevice ==> done", (in ? Target::midiIn : Target::midiOut));
	}
}

void SessionSingleton::AvailableDevice(const std::string &name, bool in)
{
	//TODO: ... Care in MainComponent::BroadCast we have boxPanel->Blink(panel, false);
	std::string midiDeviceName = (in ? "I" : "O") + name;

	// Care : not correct... The error message is "funky"
	// if (std::find(s_deviceUsages.cbegin(), s_deviceUsages.cend(), name) == s_deviceUsages.cend())

	auto it = s_deviceUsages.find(midiDeviceName);
	if (it == s_deviceUsages.cend())
	{
		RegisterMidiDevice(name, in); // It is a new device
	}
	else
	{
		// It is known device
		if (it->second.size() > 0 && (s_callback != nullptr)) // TODO : need a kind of delegate class in order to not test the pointer nullity
		{
			// And it was in used
			s_callback(midiDeviceName, dsBusyInternal);
		}
	}
}

void SessionSingleton::UnavailableDevice(const std::string &name, bool in)
{
	std::string midiDeviceName = (in ? "I" : "O") + name;
	
	auto it = s_deviceUsages.find(midiDeviceName);
	if (it == s_deviceUsages.cend())
	{
		LogThis2("SessionSingleton::UnavailableDevice ==> device %s not found, strange...", (in ? Target::midiIn : Target::midiOut));
	}
	else
	{
		if ((it->second.size() > 0) && (s_callback != nullptr))
		{
			s_callback(midiDeviceName, dsBusyExternal);
		}
	}
}
