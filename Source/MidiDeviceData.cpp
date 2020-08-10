
#include "MidiDeviceData.hpp"
#include "CommonStuffs.hpp"
#include "SessionData.hpp"

#include <vector>
#include <string>

void ArrayDiff(const std::vector<std::string> &source, std::vector<std::string> &target, bool in)
{
	auto cit = target.cbegin();
	while (cit != target.end())
	{
		if (std::find(source.cbegin(), source.cend(), *cit) == source.cend())
		{
			LogThis2("ArrayDiff ==> midi device '%s' is gone", (in ? Target::midiIn : Target::midiOut), cit->c_str());			
			SessionSingleton::UnavailableDevice(*cit, in); // Notify other threads that this one is gone
			cit = target.erase(cit);
		}
		else
		{
			cit++;
		}
	}

	cit = source.cbegin();
	while (cit != source.cend())
	{
		if (std::find(target.cbegin(), target.cend(), *cit) == target.cend())
		{
			LogThis2("ArrayDiff ==> midi device '%s' is new", (in ? Target::midiIn : Target::midiOut), cit->c_str());
			SessionSingleton::AvailableDevice(*cit, in); // Notify other threads this one is new or back
			target.emplace_back(*cit);
		}
		cit++;
	}
}

// Get rid of this pesky StringArray...
void ArrayCopy(const StringArray &source, std::vector<std::string> &target)
{
	target.clear();
	for (const auto &it : source)
	{
		target.emplace_back(it.toStdString());
	}
}

void MonitorMidiDevices::run()
{
	LogThis("MonitorMidiDevices::run ==> Session start", Target::misc);

	std::vector<std::string> imds;
	std::vector<std::string> omds;
	std::vector<std::string> tmds; // Temp copy into some standard container & type :)

	while (!threadShouldExit())
	{
		ArrayCopy(juce::MidiInput::getDevices(), tmds);
		ArrayDiff(tmds, imds, true);
		ArrayCopy(juce::MidiOutput::getDevices(), tmds);
		ArrayDiff(tmds, omds, false);

		sleep(200);
	}

	LogThis("MonitorMidiDevices::run ==> Session end", Target::misc);
}
