import serial
import matplotlib.pyplot as plt 

logfile = open("serialdata.txt", "a")

ser = serial.Serial(port='COM4', baudrate=388000, timeout=1000)
print (ser.name)
print (ser.isOpen)

def _update_address(address):
    if (address > 6000):
        return address - 6000
    elif (address < 0):
        return address + 6000
    else:
        return address

input_buffer = [None]*6000

while 1:
    Buffer_Addr_At_Trigger_raw = ser.read(16) #uint32_t : 2 bytes address, 14 bytes empty
    input_buffer_raw = ser.read(12000)

    Buffer_Addr_At_Trigger = (Buffer_Addr_At_Trigger_raw[0] + Buffer_Addr_At_Trigger_raw[1]*256) * 2

    i = 0
    while i < 6000:
        input_buffer[i] = (input_buffer_raw[2*i+1]*256 + input_buffer_raw[2*i])
        i = i+1

    t_axis = range(6000)
    y_axis = [None]*6000
   

    for i in range(6000):
        y_axis[i] = input_buffer[_update_address(i+Buffer_Addr_At_Trigger-3000)]

    plt.plot(t_axis, y_axis, linewidth = 1)
    plt.xlabel('sample #')
    plt.title("STM32 Oscilloscope Data")
    plt.show()

    logfile.write ("SOS\n")
    logfile.write("Buffer_Addr_At_Trigger = " + str(Buffer_Addr_At_Trigger) + "\n")
    for i in range(6000):
        logfile.write(str(input_buffer[i]) + "\n")

    logfile.write ("EOS\n")

logfile.close()
ser.close