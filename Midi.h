#pragma once

#include <stdio.h>
#include <portmidi.h>

#define kMaxMIDIEvents 20

typedef struct MIDIStream {
  int numMIDIEvents; //Store a count for all incoming MIDI events
  PmEvent events[kMaxMIDIEvents]; //Hold individual MIDI events
  PortMidiStream *input; //a descriptor for an open MIDI input device
} MIDIStream;


int InitMidi(MIDIStream *Stream);
int TerminateMidi(MIDIStream *Stream);
void PrintMidiDevs();