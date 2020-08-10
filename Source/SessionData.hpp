
#ifndef SESSION_DATA
#define SESSION_DATA

#include <map>
//#include <set>
#include <string>
#include <vector>

// The later status means the device was selected but is not available anymore
// enum DeviceStatus { dsFree, dsBusyInternal, dsBusyExternal }; 

// Busy means selected in at least one combo
enum DeviceStatus { dsFree, dsBusy }; // Then it can be "lost" (unplug or taken by another app) or not

// First real tentative to aim toward MVC... Well, let's do this another day !

enum class ControlContent { Device,	NibbleA, NibbleB, NibbleC, NibbleD, NibbleE, NibbleF, ByteAB, ByteCD, ByteEF, SplitAB, SplitCD, SplitEF, Mute, Solo, SE, RS, AT, RF, last_item }; // The enum to string should be built in...
const std::string controlContents[(int) ControlContent::last_item] = { "Device", "NibbleA", "NibbleB", "NibbleC", "NibbleD", "NibbleE", "NibbleF", "ByteAB", "ByteCD", "ByteEF", "SplitAB", "SplitCD", "SplitEF", "Mute", "Solo", "SE", "RS", "AT", "RF" };
typedef void(*MidiDeviceStatusChange)(const std::string &, DeviceStatus);

class SessionSingleton
{
private:
	// One devices may be used more than once. The device name hold 'I' or 'O' as first character. The value is the panel index where the device has been choosen
	// static std::multimap<std::string, int> s_deviceUsages;	
	static std::map<std::string, std::vector<int> > s_deviceUsages; // Easier like this :)
	static std::map<std::string, std::string> s_contents; // Das Model ;-)
	static int s_sourcePanel;
	static bool s_sourceIn;
	static bool s_changed;
	static MidiDeviceStatusChange s_callback;
	static void AcquireMidiDevice(int panel, const std::string &name, bool in);
	static void ReleaseMidiDevice(int panel, const std::string &name, bool in);
public:
	// Called by the controller (here MainComponent)
	static void Deserialize(const std::string &fullFileName); // From file to memory
	static void Serialize(const std::string &fullFileName); // The opposite way...
	//std::vector<std::string> RetrieveKeys(const std::string &value);
	static bool Changed() { return s_changed; }
	static std::string FindSoloDevice(bool in);
	static std::vector<int> FindPanelByDevice(const std::string &name, bool in);
	static std::string FindDeviceByPanel(int panel, bool in);
	static std::string FindControlByContent(int panel, ControlContent cc, bool in);
	//static std::string FindOutPanelById(int panel);
	static void RegisterMidiDevice(const std::string &name, bool in);
	static void RegisterEventHandler(MidiDeviceStatusChange callback);
	//static void RegisterLogHandler();
	static void RemoveMissingMidiDevice();
	static void RemoveExtraPanel(int panelCount);
	static void RemoveExtraSolo();

	// Called by the view (here FlexStuff)
	static void setSourcePanel(int value) { s_sourcePanel = value; }
	static int getSourcePanel() { return s_sourcePanel ; }
	static void setSourceIn(bool value) { s_sourceIn = value; }
	static bool getSourceIn() { return s_sourceIn; }
	static std::vector<std::string> RetrieveMidiDevices(bool in);
	static std::string Get(const std::string &key);
	static void Set(const std::string &key, const std::string &value, bool propagate);
	//static const std::string NO_DEVICE;

	// Called by the background thread...
	static void AvailableDevice(const std::string &name, bool in);
	static void UnavailableDevice(const std::string &name, bool in);
};

#endif // SESSION_DATA
