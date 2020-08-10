
-----------------------------------------
GreenWitch 3.0 - Might Message Midi Magic
-----------------------------------------

This is not a (complete) doc...

- It is foreseen to use a virtual midi driver especially under Windaube in order to add
several virtual midi devices. Tobias Erichsen loopMIDI is the one recommanded and free !
- One should name carefully its VMD's, names will be present in the MidiIn & MidiOut log files
- Midi Messages can pass through multiple In => Out iterations thanks to the fact that virtual
midi devices are circular :)
- The file names and contents of Session.ini & GreenWitch3.ini are case sensitive !
- The (@) denotes the midi messages going through
- The 'X' remove the current midi device from the combo
- The top right "icons" stands for Copy, Paste and Reset
- Only one solo device of its kind "In" / "Out"
- Merge on "Out" devices is automatic
- Multiple "In" devices is supported
- Lines are "In" / "Out" alterned on purpose, it is one "block" of operation !-)
- The bottom part of the screen list the important user messages
- Sysex message are assumed to be longer than 3 bytes and may be blocked at input
- For "exotic" messages (IE less than 3 bytes), well dedicated filter could take place...
But, for example the running status message, the juce lib fill the missing byte for us !
- One can view the three bytes as a whole or can work with their two nibbles
- Application should run fine as is on WIN-LIN-MAC and eventually Android thanks to Juce portability !-)

About the application name
--------------------------

Well, one upon a time Roli released Equator... A very powerfull and nice looking soft synth !
Although based on the excellent juce lib, Equator does not exist for Linux...
So I decided to write a juce based app called Greenwitch the most famous meridian
It becomes after Green Witch... And I'm at the third "iteration" (previous one was about 
playing with 5D midi messages to "older" devices !

About the configuration file
----------------------------

- All values are string based (key/value pairs)
- Boolean values are "1" or something else !-)
- Empty string as values shall not appears
- AutoSave is assumed since no manual load and save are implemented
- Comment line start with ';'
- The session fileName is relative to the "exe" location
- Integer values are treated as unsigned
- If it is missing it will be recreated... Hence the app will need write permission


About the log files
-------------------

- Three kinds, all optional see the log section of the configution file
- Misc is the one that is not related to midi


- About the session file
------------------------

- The session will be restored as much as possible : IE midi devices can be missing !


About the midi message flows
----------------------------

- At least one "In" midi device should be opened (else nothing happened :)
- If the message pass the filter then it will be processed by the formula
of the corresponding "Out" midi device (if present obviously).
- A complete empty filter area means no filtering at all
- A complete empty formula area means message unaltered


About input message filtering for input
---------------------------------------

- Textboxes are smaller than those for the formula's.
- All filters are implicitely linked through "and" operations
- Strings will be trimmed
- A filter have to start with one of the following : '!', '=', '<' or '>' then a decimal value
- The complete filter can be reversed by using the the RF checkbox


About input message transforming through formula for output
-----------------------------------------------------------

- All formula's can reference the following six variables : 'A' through 'F'
- Strings will be uppercased


About ongoing TODO's (eternal list)
-----------------------------------

- Move code to github ? Open source it ?



- Review completely the UI building (use a custom panel and repeat it on a scroll area) and so remove all those ugly copy/paste lines !
==> Almost done, transparancy and button images must be added



- Associated a thread to each IN device ?
==> Needed ? The handleIncomingMidiMessage is already running into another thread than the UI (hence the lock in FlexBoxItem::DrawActivityLed())



- Optimize all the transform rulez should not be computed every time
==> Need a dedicated data structure...


- Save and restore session
==> Partially done through m_sessionAutoSave but then no manual session save yet



- Handle Reverse Filter
==> Ok



- Handle Sysex (AKA message > 3 bytes)
==> Passthrough in input... Need to decide if a filter per output is needed



- Handle Message < 3 bytes
==> Juce take care of RS messages... We'll see for AT



- Don't open all devices
==> OK



- Put glyph on buttons
==> Still to do



- Transparency & resize
==> Still to do for the former, no plan yet for the later



- Review the default path for config file & log files
==> OK



- Implement OUT Combobox filtering on used devices ?
==> Not hot plug (yet)... The same goes for the midi input devices ! Though one



- Reload midi devices on each device combobox openning ?
==> Need a dedicated data structure...



- Enhance the log content : introduce log level and put it in config
==> Well not sure if log level per topic is needed... Today it is black or white :)



- Enhance the log content : put the milli seconds
==> OK, TID's are stored in bonus !


- Support hex in formula/filter ? Supported by the lib ?
==> Not supported :( Need another parser... Same goes for the missing random function



- Put the version info into the exe file
==> Today the "exe" name hold a trailing 3 :) Moreover Projucer handle version through the versionString project variable



About the conclusion
--------------------

That's all folks ! Thanks for reading all this... Hope you like this as much as I liked to program it

Sam Le Pirate TFL/TDV C++ POWA !-)