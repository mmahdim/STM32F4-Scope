# STM32F4-Scope
**7.2 MSa/s Digital Oscilloscope Using the STM32F429 Triple Interleaved ADCs**

## Specifications
Sample Rate: 7.2 MSa/s
Resolution: 12 bits
Input Range: 0-3.3v
Channel Count: 1
Buffer Size: 6000 samples
UI: Touchscreen, PC(through VCP)

## Hardware
I used the STM32F429 discovery board. <br>
### Pins

### Buttons
PC3 is used as the analog input, and PE2 is used as the trigger input. (in the demo they are both connected to the same input.) <br>
The green button on the discovery board acts as stop/start button. You would need to stop the oscilloscope in order for the touchscreen to become responsive. <br>

## Software
CubeIDE & TouchGFX have made this project available. <br>
### Buffer
Just as a normal DSO, this project also utilizes circular buffers, which enables it to capture the waveform even **before** the trigger event. Half of the DSO buffer is dedicated to the pre-trigger and the other half to the post-trigger data samples. <br>
The total circular buffer implemented has capacity for 6000 samples. <br>
### Interface
When the oscilloscope is stooped, you can press the arrow button to send the captured waveform through the COM port into the PC. <br>
You can also use the zoom in and zoom out buttons by pressing the magnifier icon, and use the scroll bar to navigate through the waveform. <br>
### To Do:
Needs some cleanup, and also some features like Volts/Div and Time/Div are not fully implemented yet. Sample rate is fixed to 7.2 Msps. Might do the cleanup and the complete the project if it received enough attention. <br>
I also have written a Python application which receives the waveform from the USB VCP and displays the waveform on PC.

## Installation
You need to open the .project file in the stm32cubeIDE and then program it to your chip. <br>
In order to use the PC interface python program, change the COM port in the python source code to your desired COM port.

## Demo
Here's a youtube Demo:<br>
https://www.youtube.com/watch?v=pv_ZHL44sbU<br>
(in the video the wires are loose and the distortion is due to that.)<br>
### Touchscreen Interface Demo
![image](https://user-images.githubusercontent.com/8644346/178038125-e4bdb67a-f545-4dda-a86f-d97f4b7a3c98.png) <br>
### PC Interface Demo
![image](https://user-images.githubusercontent.com/8644346/186186668-074957f8-7df2-4b6a-9288-2991fcd593d2.png)


## How you can support this project
Donations are welcomed.<br>
BTC:<br>
34Hf7Xb4twf2kK4PrSRhjoiNXm7kTzeggs

ETH (ERC20):<br>
0xb8fdebde6733df0189404ce845766d27b81df8bc
