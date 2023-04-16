import serial
import matplotlib.pyplot as plt
import numpy as np
from genref import genRef
import time

#best for step: Kp = 400, Ki = 6, Kd = 7000
#best for cubic, hold: Kp = 750, Ki = 15, Kd = 5000
# 0 0 1 360 2 90 3 90 4 -180 5 0 6 0 7 -45 8 -180 9 0

ser = serial.Serial('/dev/tty.usbserial-0232C119', 230400)  # open serial port
quit = False

# print commands
print("Commands:")
print("c: get encoder count (counts)")
print("d: get encoder count (deg)")
print("e: zero encoder")
print("r: get mode")
print("a: read current (mA)")
print("f: set PWM")
print("p: depower motor")
print("g: set current gains")
print("h: get current gains")
print("k: test current gains")
print("i: set position gains")
print("j: get position gains")
print("l: hold angle")
print("m: generate step trajectory")
print("n: generate cubic trajectory")
print("o: test trajectory")
print("b: manual control")
print("q: quit")

Ival = []
Iref = []
Pval = []
Pref = []
ref = []

while not quit:
    #send commnad to pic

    command = input("Input command: ")     # read command from user
    ser.write(f'{command}\n'.encode('utf-8'))     # send command to pic

    match command:

        case 'c': #get encoder count (counts)
            print(ser.readline().decode("utf-8"), end='')
            
        case 'd': #get encoder count (deg)
            print(ser.readline().decode("utf-8"), end='')
        
        case 'e':
            print("Zeroing encoder")

        case 'r': #get mode
            print(ser.readline().decode("utf-8"), end='')
            
        case 'a': #get current
            print(ser.readline().decode("utf-8"), end='')

        case 'f': #set PWM
            PWM = input("Input PWM: ")     # read PWM from user
            ser.write(f'{float(PWM)}\n'.encode('utf-8'))     # send command to pic 

        case 'p': #depoer motor
            print("Depowering motor") 

        case 'g': #set current gains
            gains = input("Input Kp, Ki: ").split(" ")     # read PWM from user
            ser.write(f'{float(gains[0])} {float(gains[1])}\n'.encode('utf-8'))    # send command to pic
            print("Setting current gains")

        case 'h': #get current gains
            print(ser.readline().decode("utf-8"), end='')

        case 'k': #test current gains
            testing = True
            while testing:
                message = ser.readline().decode("utf-8").split(" ")
                Ival.append(float(message[1]))
                Iref.append(float(message[2]))
                if(message[0] == '1'):
                    testing = False

            ser.write(b'h\n') #get current gains for plot title
            gains = ser.readline().decode("utf-8")
            #error = np.subtract(Ival, Iref) #error
            #score = np.sum(np.square(error)) #sum of squared error
            score = sum(map(lambda x,y: abs(x-y), Ival, Iref)) / len(Ival) #sum of absolute error

            pts = range(len(Ival))
            plt.plot(pts, Ival, label = 'Ival')
            plt.plot(pts, Iref, label = 'Iref')
            plt.legend(loc="upper left")
            plt.xlabel("Time")
            plt.ylabel("Current (mA)")
            plt.title(f"Current Test, {gains} Score: {score}")
            plt.show()
            Ival = []
            Iref = []
 
        case 'i': #set posn gains
            gains = input("Input Kp, Ki, Kd: ").split(" ")     # read PWM from user
            ser.write(f'{float(gains[0])} {float(gains[1])} {float(gains[2])}\n'.encode('utf-8'))    # send command to pic
            print("Setting Position gains")

        case 'j': #get posn gains
            print(ser.readline().decode("utf-8"), end='')

        case 'l': #hold angle
            angle = input("Input angle: ")     # read PWM from user
            ser.write(f'{float(angle)}\n'.encode('utf-8'))    # send command to pic
            print("Holding angle")

        case 'm': #generate step trajectory
            ref = genRef('step')
            ser.write(f'{len(ref)}\n'.encode('utf-8'))
            for i in range(len(ref)):
                time.sleep(0.0005)
                ser.write(f'{ref[i]}\n'.encode('utf-8'))
            
        case 'n': #generate cubic trajectory
            ref = genRef('cubic')
            ser.write(f'{len(ref)}\n'.encode('utf-8'))
            for i in range(len(ref)):
                time.sleep(0.0005)
                ser.write(f'{ref[i]}\n'.encode('utf-8'))
            
        case 'o': #test trajectory
            testing = True
            while testing:
                message = ser.readline().decode("utf-8").split(" ")
                Pval.append(float(message[1]))
                Pref.append(float(message[2]))
                if(message[0] == '1'):
                    testing = False

            score = sum(map(lambda x,y: abs(x-y), Pval, Pref)) / len(Pval) #sum of absolute error
            time.sleep(0.005)
            ser.write(b'j\n') #get posn gains for plot title
            gains = ser.readline().decode("utf-8")
            pts = range(len(Pval)) 
            plt.plot(pts, Pval, label = 'Pval')
            plt.plot(pts, Pref, label = 'Pref')
            plt.legend(loc="upper left")
            plt.xlabel("Time")
            plt.ylabel("Angle (deg)")
            plt.title(f"Posn Test, {gains}Score: {score}")
            plt.show()
            Pval = []
            Pref = []
        
        case 'b': #manual control
            print("Manual control")

        case 'q': #quit
            quit = True
            continue


ser.close()             # close port