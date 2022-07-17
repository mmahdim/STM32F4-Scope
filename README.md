# STM32F4-Scope
## 7.2 MSa/s Digital Oscilloscope Using the STM32F429 Triple Interleaved ADCs

I used the discovery board. CubeIDE & TouchGFX are used to build this project.

Just as a normal DSO, this project also utilizes circular buffers, which enables it to capture the waveform even **before** the trigger event. Half of the DSO buffer is dedicated to the pre-trigger and the other half to the post-trigger data samples. <br>
Needs some cleanup, and also some features like Volts/Div and Time/Div are not fully implemented yet. Sample rate is fixed to 7.2 Msps. Might do the cleanup and the complete the project if it received enough attention.

I also have written a Python application which receives the waveform from the USB VCP and displays the waveform on PC, which I will upload if the project received public attention.

Here's a youtube Demo:<br>
https://www.youtube.com/watch?v=pv_ZHL44sbU<br>
(in the video the wires are loose and the distortion is due to that.)<br>
![image](https://user-images.githubusercontent.com/8644346/178038125-e4bdb67a-f545-4dda-a86f-d97f4b7a3c98.png)

Donations are welcomed.<br>
BTC:<br>
34Hf7Xb4twf2kK4PrSRhjoiNXm7kTzeggs

ETH (ERC20):<br>
0xb8fdebde6733df0189404ce845766d27b81df8bc
