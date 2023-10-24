#pragma once

#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include "OpenAudio.h"
#include "Midi.h"


#define kMstoSamples (kSampleRate/1000)

//Max grain size will be used so we can create a table for the grains whose value will not vary, only the values being written to the table will vary
#define kMaxGrainSize (int) (200 * kMstoSamples) 
#define kMaxGrainRate 1000 * kMstoSamples //This is in millisecond, so the current max is 1 second


#define kMaxGrainVoices (int) 100 //this value will be used to limit how many grain voices there are, this value will be looped through in the process function and all current grain voices will be added


typedef struct GrainMidi {
    float GrainSize; //Between 0 and 200 ms
    float GrainRate; //Between 0 and 200 ms
    float Playback; //Between (possibly negative values?) 0 and 3
    float WindowAlpha; //Between 0.0 and 1.0
} GrainMidi;

//this data structure will be used to create the grains when a grain start time calls the create grain function, this must include all data necessary to complete the functions
typedef struct Grains{
    float table[kMaxGrainSize]; //This table will hold the grain
    int GrainVoice;
    unsigned long int index; //This value will be at what index point (i) this grain should start at, will be used by the final process function loop to know when to start copying this data into the final buffer
    float * fileData;
    MIDIStream * Stream;
    GrainMidi Parameters;
} Grains;

typedef struct AllGrain{
  Grains Grains;
  GrainMidi GrainMidi;
} AllGrain;


float GetGrain(int index, int filelength, int grainVoice, Grains *data);
void MakeGrain(int index, float playbackIndex, int filelength, Grains *data, GrainMidi *Parameters);
void InitGrains(Grains *grain, int index);
void InitGrainMidi(GrainMidi *data);
float GetAmp(int index, Grains *data);
