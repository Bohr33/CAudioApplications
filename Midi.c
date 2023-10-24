#include "Midi.h"

int InitMidi(MIDIStream *Stream){

  int DeviceID;
    PmError err;
    //Initialize Port MIDI and check for error
  err = Pm_Initialize();
  if(err != pmNoError){
    printf("Error: Pm_Initialize() failed with %s\n", Pm_GetErrorText(err));
    return 1;
  }
  printf("Please Select a MIDI ID ");
  scanf("%d", &DeviceID);
    //Open the midi stream and check for error in opening
  err = Pm_OpenInput(&Stream->input, DeviceID, NULL, 512L, NULL, NULL);
    if(err != pmNoError){
      printf("Error: Pm_OpenInput() failed. %s\n", Pm_GetErrorText(err));
      return 1;
    }
    return 0;
}

int TerminateMidi(MIDIStream *Stream){

    PmError err;
        //Close input MIDI stream
    if(Stream->input){
        err = Pm_Close(Stream->input);
        if(err != pmNoError){
        printf("Error: Pm_Close() failed. %s\n", Pm_GetErrorText(err));
        return 1;
        }
    }

    //Terminate Port MIDI
    err = Pm_Terminate();
    if(err != pmNoError){
        printf("Error: Pm_Terminate() failed with %s\n",Pm_GetErrorText(err));
        return 1;
    }
  return 0;
}

void PrintMidiDevs(){

    int numDevices = Pm_CountDevices();
    const PmDeviceInfo *pDeviceInfo; 
    PmDeviceID curDeviceID;

     for(curDeviceID = 0; curDeviceID < numDevices; curDeviceID++){
    pDeviceInfo = Pm_GetDeviceInfo(curDeviceID);
    printf("--------------------------------------------\n");
    printf("ID: %d, Name: %s, ",curDeviceID,pDeviceInfo->name);
    printf("MIDI API: %s\n",pDeviceInfo->interf);
    printf("Max MIDI outputs: %d\t",pDeviceInfo->output);
    printf("Max MIDI inputs: %d\n\n",pDeviceInfo->input);
  }
}

