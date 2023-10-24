#include "OpenAudio.h"

SNDFILE *inFile = NULL;
SF_INFO info;
float *buffer;

//Opens the audio file being used to granulate and stores it into buffer using libsnd, Soundfile is passed in main function
int OpenFile(SoundFile *sndfile){

     //fill SF_INFO info data with 0's
    memset(&sndfile->info, 0, sizeof(SF_INFO));
    
    //open audio file and check for error
    sndfile->file = sf_open(kAudioFile, SFM_READ, &sndfile->info);

    if(!sndfile->file){
            printf ("Error : could not open file\n");
            puts (sf_strerror (NULL));
            return 1;
    }
    
    //Check if the file format is in good shape
  if(!sf_format_check(&sndfile->info)){	
    sf_close(sndfile->file);
		printf("Invalid encoding\n");
		return 1;
	}

    //print out information about opened sound file
    printf("Sample rate for this file is %d\n", sndfile->info.samplerate);
	  printf("A number of channels in this file is %d\n", sndfile->info.channels);
    printf("A number of frames in this file is %lld\n", sndfile->info.frames);
    printf("Duration is %f\n", (double)sndfile->info.frames / sndfile->info.samplerate);

    //reads the data from the provided file into our buffer "buffer"
    // sf_count_t readcount = sf_read_float(inFile, buffer, sndfile->info.frames);

    //closes the infile after copied into buffer
    // sf_close(inFile);
    return 0;
}

//initializes portaudio and checks for error
int initPortAudio(){
    PaError PaOpenError = Pa_Initialize();
    if(PaOpenError != paNoError)
    {
        printf("Error initializing port audio");
        return 1;
    }
    return 0;
}

//print available output devices
void printDevices(){
  //Get number of port audio devices available
  PaDeviceIndex numDevices = Pa_GetDeviceCount(); 
  PaDeviceIndex curDeviceID;

  const PaDeviceInfo *pDeviceInfo; 
  const PaHostApiInfo *pHostApiInfo;

  //Iterate over each device and print out information about them
  for(curDeviceID = 0; curDeviceID < numDevices; curDeviceID++){
    pDeviceInfo = Pa_GetDeviceInfo(curDeviceID);
    pHostApiInfo = Pa_GetHostApiInfo(pDeviceInfo->hostApi);
    printf("--------------------------------------------\n");
    printf("ID: %d, Name: %s, ", curDeviceID, pDeviceInfo->name);
    printf("API name: %s\n", pHostApiInfo->name);
    printf("Max output channels: %d\t", pDeviceInfo->maxOutputChannels);
    printf("Max input channels: %d\n\n", pDeviceInfo->maxInputChannels);
  }
}

//declares the values for the parameters in the outputparameter structures which is used for opening the audio stream
void configureOutput(int deviceIndex, AudioStream *audiostream){
    memset(&audiostream->outputParameters, 0, sizeof(PaStreamParameters));
    audiostream->outputParameters.channelCount = kNumChannels;
    audiostream->outputParameters.device = deviceIndex;
    audiostream->outputParameters.sampleFormat = paFloat32;
    audiostream->outputParameters.suggestedLatency = 0.0;
}

//Opens the audio stream, stream, callback function, and userdata is passed in, errors are checked for
int OpenAudioStream(AudioStream *audiostream, PaStreamCallback *CallbackFunction, void *userdata){
    //opens stream and assigns an error value to the error variable
    PaError openError = Pa_OpenStream(
        &audiostream->stream,
        NULL,
        &audiostream->outputParameters,
        kSampleRate,
        kNumFramesPerBuffer,
        paNoFlag,
        CallbackFunction,
        userdata
    );
    //error variable is checked for an error, if one is found, port audio is closed and the function is exited
    if (openError != paNoError)
    {
        printf("Error opening port audio stream\n");
        closePortAudio();
        return 1;
    }
    return 0;
}

//Starts the actual audio stream and checks for error when starting
int StartAudioStream(AudioStream *audiostream){
    //Start port audio streaming
  PaError error = Pa_StartStream(audiostream->stream);
  //Check for error in opening stream
  if(error != paNoError){
    printf("Error: Failed to start port audio stream. %s\n",Pa_GetErrorText(error));
    return 1;
  }
  //If no error, render audio until user input
  else {
    printf("Rendering audio... Press enter to stop streaming audio\n");
    getchar();
    getchar();
    error = Pa_StopStream(audiostream->stream);
    if(error != paNoError){
      printf("Error: Failed to stop port audio stream. %s", Pa_GetErrorText(error));
      return 1;
    }
  }
  return 0;
}

//closes port audio and checks for errorf
int closePortAudio(){
    PaError PaCloseError = Pa_Terminate();
    if(PaCloseError != paNoError){
        printf("Error in closing port audio");
        return 1;
    }
    return 0;
}
