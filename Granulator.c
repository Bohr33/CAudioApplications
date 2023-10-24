#include "Granulator.h"
#include "OpenAudio.h"

//initialize the grain structures; this is for looped for each grain voice in the main function
void InitGrains(Grains *grain, int index){
    memset(grain->table, 0, kMaxGrainSize * sizeof(float));
    grain->index = 0;
    grain->GrainVoice = index;
    InitGrainMidi(&grain->Parameters);
}

void InitGrainMidi(GrainMidi *data){
    data->GrainRate = 30 * kMstoSamples;
    data->GrainSize = 10 * kMstoSamples;
    data->Playback = 1;
    data->WindowAlpha = .8;
}


//index value passed must continue to grow and not reset like the callback index 'n'. this value must increase by 1 outside the function when it is being called
//GetGrain will take the voice value in and retrieve the sample data store in the "makeGrain" function
float GetGrain(int index, int filelength, int grainVoice, Grains *data){

    float a = 0;
    a =  data[grainVoice].table[index - data[grainVoice].index];

    //probably don't need the code below, works without it, don't know why i had it
    //I think it was another check to make sure we don't load past the audio file, but thats done in the
    //make grain function
    // if(data[grainVoice].index <= index && playbackIndex < filelength - kMaxGrainSize){
    //     // printf("%f\n", data[grainVoice].table[index - data[grainVoice].index]);
    //     a =  data[grainVoice].table[index - data[grainVoice].index];

    // }else{
    //     a = 0;
    // }
    //Multiplies the returned value by its appropriate amp value based on the table in the window structure
    a = a * GetAmp(index - data[grainVoice].index, data);
    return a;
}

void MakeGrain(int index, float playbackIndex, int filelength, Grains *data, GrainMidi *Parameters){

    //copies the value of the fileData array at the index (which is the point in the file in which this grain will be taken from) into the grain structure
    //This if statement makes sure grains aren't being made that exceed the length of the file
    int grainSize = (int) Parameters->GrainSize;
    
    int p = (int) playbackIndex;

    for(int i = 0; i < grainSize; i++){

            if(p < (filelength - grainSize)){
                data->table[i] = data->fileData[(int) (i + p)];
                // printf("Less than\n");
            }else{
                if(i + p < filelength){
                    data->table[i] = data->fileData[(int) (i + p)];
                }else{
                    data->table[i] = data->fileData[(int) (i + p - filelength)];
                }
            }
        }

    //copies the index at which this grain was made so the GetGrain can reference this to know when to begin copying the data within it.
    data->index = index;

}

float GetAmp(int index, Grains *data){

    float a;
    if (index >= data->Parameters.GrainSize)
    {
        return 0.0;
    }

     if(index < (data->Parameters.WindowAlpha * data->Parameters.GrainSize)/2){
            a = .5 *(1.0 - cos((2.0 * M_PI * (float) index)/(data->Parameters.WindowAlpha * data->Parameters.GrainSize)));
        } else if(index<= ((data->Parameters.GrainSize - ((data->Parameters.WindowAlpha*data->Parameters.GrainSize)/2.0)))){
            a = 1.0;
        }else if(index< data->Parameters.GrainSize){
            a = 0.5 * (1 + cos(((2.0 * M_PI)/(data->Parameters.WindowAlpha * data->Parameters.GrainSize)) * ((float)index- data->Parameters.GrainSize + ((data->Parameters.WindowAlpha * data->Parameters.GrainSize)/2.0))));
        }
    
    return a;
}