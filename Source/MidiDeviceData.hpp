
#ifndef MIDI_DEVICE_DATA
#define MIDI_DEVICE_DATA

#include "../JuceLibraryCode/JuceHeader.h"

//#include <map>
//#include <string>
//#include <vector>

//TODO: Put incoming / outgoing device handling here ?
// https://forum.juce.com/t/detect-midi-device-connect-disconnect/8856/3
// Use a background thread that calls MIDIGetNumberOfDestinations & MIDIGetNumberOfSources ? Mac only !

/*

Lexicon
-------

Live device : midi device present currently ! Could be either busy or free

Session device : midi device used previously ! Could be no more available

==> Device can be red (not here or unavailable or unplug or ...) or white !

==> Then some white can be selected in combos

Use cases
---------

UI : should be notified when a device become unavailable (if it is possible)
UI : should be notified when a device vanish
UI : should ask for the available devices at each combo openning (or we do it all the time async ?)

*/

class MonitorMidiDevices : public Thread
{
public:
	void run() override;
	MonitorMidiDevices() : Thread("mmd") {}
};

#endif // MIDI_DEVICE_DATA
