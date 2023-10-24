#pragma once

#include <stdio.h>
#include <string.h>
#include <sndfile.h>
#include <stdlib.h>
#include <portaudio.h>

//Audio file is named here, must change this is if another file is to be used
#define kAudioFile "Guitar.wav"
#define kNumFramesPerBuffer 512
#define kSampleRate 44100.0
#define kNumChannels 1

//this structure holds data for portaudio, it holds the stream address, as well as info for input and output parameters (although input parameters are not being used and should probably be deleted)
typedef struct AudioStream {
  PaStream *stream; //For port audio streaming
  PaStreamParameters inputParameters; //Parameters for input of a stream
  PaStreamParameters outputParameters; //Parameters for output of a stream
} AudioStream;

typedef struct SoundFile {
    SNDFILE *file;
    SF_INFO info;
} SoundFile;


int OpenFile(SoundFile *sndFile);
int initPortAudio();
void printDevices();
void configureOutput(int deviceIndex, AudioStream *audiostream);
int OpenAudioStream(AudioStream *audiostream, PaStreamCallback *Callbackfunction, void *userdata);
int StartAudioStream(AudioStream *audiostream);
int closePortAudio();