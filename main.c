#include "OpenAudio.h"
#include "Granulator.h"
#include "Midi.h"

#define kOutputDeviceIndex 11

//TO DO - figure out what the max sample size is. Currently BUS error 10 is an issue on any file seemingly larger than 10 seconds.
// Since it seems to respond when MIDI happens, I assume it has something to do with how sound file data is passed to make a new grain
//However it does now work with stereo files

//Kept trying to figure out what causes the bus error. Have gotten nowhere, but did find out that sometimes it will stop making grains when changing grain rate,
//something is wrong there.

//Grainulator by Ben Ward//
//This program will granulate an input audio file. The grainulation controls must be done be a midi cc messages/
//The four parameters being control are 
//1. Grain Size: MIDI CC 20
//2. Grain Rate: MIDI CC 21
//3. Playback Rate: MIDI CC 22
//4. Window Alpha: MIDI CC 23

//These parameters will probably move from this spot into their own modular base. Both are in units of milliseconds and should be converted to be understood in terms of samples
// #define kGrainSize 1
// #define kGrainRate 2

//Audio render callback function
int renderCallback(
  const void *input,
  void *output,
  unsigned long frameCount,
  const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags,
  void *userData
);

void granulateProcess(Grains *dataInput, float *outputbuffer, unsigned long frames);
void getMidiInput(Grains *grains);
 
SoundFile sndFile;
SNDFILE *outFile = NULL;
//Declare windowFile structure
GrainMidi MidiGrainData;

    
int main(void){

    SF_INFO sf_info;
  
    int AudioDeviceIndex;

    //Create midistream structure
    MIDIStream midistream;
    //Print Midi Devices
    PrintMidiDevs();
    //initalize port midi and opening midi stream
    if(InitMidi(&midistream)) return 1;
    
    //opens the file to granulate and stores it in sndFile
    if(OpenFile(&sndFile)) return 1;


    //Create and store audio data into array so we can include that in our graindata structure and pass it into the callback function
    float * fileArray;
     float * monoFileArray;
    fileArray = (float *) malloc(sndFile.info.frames * sizeof(float) * kNumChannels);
    monoFileArray = (float *) malloc(sndFile.info.frames * sizeof(float) * kNumChannels);


    sf_read_float(sndFile.file, fileArray, sndFile.info.frames * kNumChannels);
    sf_close(sndFile.file);

    printf("Here\n");
    int k = 0;
    //if stereo, copy as mono file and continue
    if(kNumChannels != 1){
      for(int i = 0; i < sndFile.info.frames; i++){
        // printf("i = %d\n",i);
        monoFileArray[i] = (fileArray[k] + fileArray[k + 1])/2;
        k += kNumChannels;
        // printf("k = %d\n", k);
    }
    }else{
      for(int i = 0; i < sndFile.info.frames; i++){
        monoFileArray[i] = fileArray[i];
        // printf("k = %d\n", k);
    }
    } 

    //initialize the grain stuctures; these will hold the actual grain data; For some weird reason I have to make way more of these than I think I need to avoid segmentation Fault

    Grains grains[200];

      


    for(int i = 0; i < kMaxGrainVoices; i++){
      InitGrains(&grains[i], i);
      grains[i].fileData = monoFileArray;
    }

    free(fileArray);
    free(monoFileArray);


    grains[0].Stream = &midistream;
    
    //Initailize 
    InitGrainMidi(&MidiGrainData);

    AudioStream audiostream;

    if(initPortAudio()) return 1;

    printDevices();
    
    printf("Please Select an Output Audio Device from list above\n");
    scanf("%d", &AudioDeviceIndex);

    configureOutput(AudioDeviceIndex, &audiostream);

    if(OpenAudioStream(&audiostream, renderCallback, &grains)) return 1;

    if(StartAudioStream(&audiostream)) return 1;

    if(closePortAudio()) return 1;

    if(TerminateMidi(&midistream)) return 1;

   

    return 0;
};

int renderCallback(
  const void *input,
  void *output,
  unsigned long frameCount,
  const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags,
  void *userData 
){
 //Convert user data so that we can use it in the callback
  Grains *grainsCopy = (Grains *) userData; //Copy Passed user data into a new grains type
  
  unsigned long numFrames = frameCount * kNumChannels; //Multiples the frameCount provided in calling the renderCallback function by the constant kNumChannels provided in the OpenAudio.h module, I believe this represents the number of frames of the buffer (512 for 1 channel)
   
  //float *outputBuffer = (float *) output; outBuffer currently holds the file 
  getMidiInput(grainsCopy);
  granulateProcess(grainsCopy, output, numFrames);

  return 0;
}

void granulateProcess(Grains *dataInput, float *outputbuffer, unsigned long numFrames){

    static sf_count_t i = 0;
    static float p = 0; //this will be the value that contains the real counter/ which will be i multiplied by the playback rate
    static float j = 0;
    static int voiceCount = 0; //this index will keep track of the voices, it will be increased each time a new grain is made
    //this value will be in the buffer loop and will store the output of the GetGrain Function
    float sample = 0;

        for(sf_count_t n = 0; n <= numFrames; n++){
          
            p += dataInput[0].Parameters.Playback/kNumChannels;

            //this if statement determines if a new grain should be made; decides that based on the master index i and the grain rate
            if( (int) (i % (int) dataInput[0].Parameters.GrainRate) == 0){

                MakeGrain(i, (int) (p + .5), sndFile.info.frames, &dataInput[voiceCount], &dataInput[0].Parameters);
                //printf("Grain Made!");
                //this increments the voiceCount variable when a new grain is made and resets it when grainVoices have run out
                voiceCount ++;
                if(voiceCount >= kMaxGrainVoices){
                  voiceCount = 0;
                }
              }

           for(int grainVoice = 0; grainVoice < kMaxGrainVoices; grainVoice++){
            //Loop through all grainvoices and add them together
                 sample += ((GetGrain(i, sndFile.info.frames, grainVoice, dataInput)) * (1.0/((float)3)));

              }
              outputbuffer[n] = sample;

              sample = 0;
              i++;
              
              //When "i" reaches the end of the file, reset i and reset the grain voice index so the previous large values aren't carried over into the new GetAmp function
              if(p >= sndFile.info.frames){
                i = 0;
                p = 0;
                for(int grainVoice = 0; grainVoice < kMaxGrainVoices; grainVoice++){
                  dataInput[grainVoice].index = 0;
                  for(int k = 0; k < kMaxGrainSize; k ++){
                    dataInput[grainVoice].table[k] = 0;
                  }
                }
              }
          }
};

//this value needs to update values in the grain function based on the midi input so they can be read in the process function
void getMidiInput(Grains *grains){

  //Assign stream in first grain array to a stream we can reference within this function
  MIDIStream Stream = *grains[0].Stream;
  static unsigned char status;
  static unsigned char MidiCC;
  static unsigned char MidiValue;
  static int numMIDIEvents = kMaxMIDIEvents;

  //questions if there is data to be read, if so read through the data with for loop
  if(Pm_Poll(Stream.input)){
    numMIDIEvents = Pm_Read(Stream.input, Stream.events, kMaxMIDIEvents);

      for(int i = 0; i < numMIDIEvents; i++){
        //assign status byte value to status variable
        status = Pm_MessageStatus(Stream.events[i].message);
      
        //if a cc message change is detected in MIDI channel 1...
        if(status == 0xB0){
            MidiCC = Pm_MessageData1(Stream.events[i].message);
            MidiValue = Pm_MessageData2(Stream.events[i].message);

          switch(MidiCC){
            case 20:
            grains[0].Parameters.GrainSize = (((MidiValue + 1.)/128.) * (kMaxGrainSize));
            break;

            case 21:
            grains[0].Parameters.GrainRate =  ((( MidiValue + 1.)/128.) * (kMaxGrainRate));
            
            break;

            case 22:
            grains[0].Parameters.Playback =  (MidiValue * (2./128.));
            break;

            case 23:
            grains[0].Parameters.WindowAlpha =  MidiValue * (1./128.);
            break;
          }
        }
      }
  }
}