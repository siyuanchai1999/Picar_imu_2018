import serial
import csv
import numpy as np
import RPi.GPIO as GPIO
#import pandas as pd
import time
import os
from subprocess import call
ser = serial.Serial('/dev/ttyACM0',115200)
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(18,GPIO.OUT)    
GPIO.output(18,GPIO.LOW)

listA_x = [0]
listA_y = [0]
listAcc_E =[0]
listAcc_N =[0]
listG = []
listH = [0]
listV_E = [0]
listV_N = [2.23]
listD_E = [-1]
listD_N = [0]
 
sumA_x = 0.0
sumA_y = 0.0
sumG = 0.0
sumH = 0.0
sumD_x = 0.0
sumD_y = 0.0
index = 1

time.sleep(6) 
ser.write('!'.encode())

def process_imu_data(line):
    delta_t = 0.05
    str_line = line.decode()
    if(str_line[0] == 'A'):
        comma_pos = str_line.index(',')
        A_x = float(str_line[1:comma_pos])
        A_y = float(str_line[comma_pos+1:-2])
        global sumA_x
        global sumA_y
        sumA_x = sumA_x + A_x
        sumA_y = sumA_y + A_y
        listA_x.append(A_x)
        listA_y.append(A_y)
         
    elif(str_line[0] == 'G'):
        G = float(str_line[1:-2])
        global sumG
        sumG = sumG + G
        listG.append(G)
    elif(str_line[0] == 'H'):
        H = float(str_line[1:-2])
        global sumH
        global index
        sumH = sumH + H
        H = H - 90
        H = H/180* np.pi
        listH.append(H)
        accele_E = listA_y[index] *np.cos(H) - listA_x[index]* np.sin(H) #convet IMU coordinates to world E-N coordinate
        accele_N = listA_y[index] *np.sin(H) + listA_x[index]* np.cos(H)
        listAcc_E.append(accele_E)
        listAcc_N.append(accele_N)
          
        vel_E = listV_E[index - 1] + (accele_E + listAcc_E[index-1])/2 * 9.814 *delta_t #update velocity
        vel_N = listV_N[index - 1] + (accele_N + listAcc_N[index-1])/2 * 9.814 *delta_t
        listV_E.append(vel_E)
        listV_N.append(vel_N)

        dis_E = listD_E[index - 1] + (vel_E+listV_E[index-1])/2 * delta_t  #update displacement
        dis_N = listD_N[index - 1] + (vel_N+listV_N[index-1])/2 * delta_t
        listD_E.append(dis_E)
        listD_N.append(dis_N)

        index = index + 1
    elif(str_line[0] == 'T'): 
        if(int(str_line[1:-2]) == 70):
            outputName = 'data0.csv'
            while(os.path.isfile(outputName)):
                dotPos = outputName.index('.')
                outputName = outputName.replace(outputName[4:dotPos],str(int(outputName[4:dotPos]) +1))
            try:
                with open(outputName,'w') as mycsv:
                    wr = csv.writer(mycsv, quoting = csv.QUOTE_ALL)
                    #listA_x.append(sumA_x)
                    wr.writerow(listA_x)
                    #listA_x.pop()
                     
                    #listA_y.append(sumA_y)
                    wr.writerow(listA_y)
                    #listA_y.pop()

                    wr.writerow(listAcc_E)
                    wr.writerow(listAcc_N)

                    wr.writerow(listV_E)
                    wr.writerow(listV_N)

                    wr.writerow(listD_E)
                    wr.writerow(listD_N)
                    
                    #listG.append(sumG)
                    wr.writerow(listG)
                    #listG.pop()
                    
                    #listH.append(sumH)
                    wr.writerow(listH)
                    #listH.pop()
                    mycsv.flush()
                    print("done output")
            except Exception as e:
                outputName = outputName.replace('data','error')
                outputName = outputName.replace('.csv','.txt')
                with open(outputName, 'w') as mylog:
                    log.write('ERROR LOG\n')
                    log.write(e)
            return 0
    else:
        print("err message")
    return 1

con = 1
while (con == 1):
    con = 1
    if(ser.in_waiting >0):
        line = ser.readline()
        if(line != b''):
            print(line)
            con = process_imu_data(line)
            
GPIO.output(18,GPIO.HIGH)
time.sleep(1)
GPIO.output(18,GPIO.LOW)
time.sleep(1)
GPIO.output(18,GPIO.HIGH)
time.sleep(1)
GPIO.output(18,GPIO.LOW)
time.sleep(1)
GPIO.output(18,GPIO.HIGH)
time.sleep(1)
GPIO.output(18,GPIO.LOW)
time.sleep(1)
GPIO.output(18,GPIO.HIGH)
time.sleep(1)
GPIO.output(18,GPIO.LOW)
time.sleep(1)
GPIO.output(18,GPIO.HIGH)
time.sleep(6)
call("sudo shutdown -h now", shell= True)

 
