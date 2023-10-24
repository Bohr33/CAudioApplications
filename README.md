##Granulator Manual

- To start, place your desired granulation file into the folder that contains the code modules (Note* - currently only supports mono file granulation)

- Compile the code by typing 'make' into the terminal. Then run with ./main

- After this initial compile, view the printed visible Audio Output Units and MIDI inputs

- In the terminal, you will prompted to select a MIDI device from the list above, and an audio playback device, once these are selected, it will begin processing

- There are 4 available parameters to control which require specific midi numbers. GrainSize = CC# 22, GrainRate = CC#23, Playback Rate = CC#24, WindowAlpha = CC#25. **NOTE** These values can be changed by altering the case values at the bottom of the main.c file in the 'getMidiInput' function.

- audio should play and loop, use your selected midi control knobs to alter the sound parameters.

- Push enter while in the terminal to exit the code.
