##Granulator Manual

- To start, place your desired granulation file into the folder that contains the code modules (Note\* - currently only supports mono file granulation)

- Compile the code by typing 'make' into the terminal. Then run with ./main

- After this initial compile, view the printed visible Audio Output Units and MIDI inputs

- you will be prompted to select a MIDI device ID, as well as an Audio Device ID from the provided list. Once these are selected, it will begin processing.

- There are 4 available parameters to control which require specific midi numbers. GrainSize = CC# 22, GrainRate = CC#23, Playback Rate = CC#24, WindowAlpha = CC#25. **NOTE** These values can be changed by altering the case values at the bottom of the main.c file in the 'getMidiInput' function.

- Once these are set, recompile the code with 'make' and execute with './main'.

- Now audio should play and loop, use your selected midi control knobs to alter the sound parameters.

- Push any button while in the terminal to exit the code.

- to upload your own sound file, enter the 'OpenAudio.h' file and replace the kAudioFile string with whatever audio file you'd like to use (must place the file in the project directory), then change the kSamplingRate to the file's sampling rate, and also the number of channels.
