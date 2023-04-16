#best vals: Kp, Ki: 0.2 0.1

import serial, sys
import matplotlib.pyplot as plt

#get Kp, Ki
#Kp, Ki = [float(x) for x in input("Input Kp, Ki: ").split()]
Kp = sys.argv[1]
Ki = sys.argv[2]

ser = serial.Serial('/dev/tty.usbserial-0232C119', 230400)  # open serial port

ser.write(f'{Kp} {Ki}\n'.encode('utf-8'))     # send Kp, Ki to pic

#initalise arrays to store e, ref
adcArr = []
refArr = []
eArr = []
dArr = []


while True:
    bs = ser.readline() #read serial line from pic
    i, adc, ref, e, d = [int(x) for x in bs.decode("utf-8").split(" ")] #parse data 
    adcArr.append(adc) #store e, ref
    refArr.append(ref)
    eArr.append(e)
    dArr.append(d) 
    if (i == 0): #if at end of data, break
        break


#plot data
pts = range(200)
plt.plot(pts, adcArr, label = 'adc')
plt.plot(pts, refArr, label = 'ref')
#plt.plot(pts, eArr, label = 'Error')
#plt.plot(pts, dArr, label = 'Duty Cycle')
plt.legend(loc="upper left")
plt.xlabel("Time")
plt.ylabel("ADC Ticks")
plt.title(f'Kp: {Kp}, Ki: {Ki}')
plt.show()

ser.close()             # close port