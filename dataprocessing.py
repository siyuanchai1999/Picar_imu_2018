import serial
import csv
import numpy as np
#import pandas as pd
import time
ser = serial.Serial('/dev/ttyACM0',9600)
 


listA_x = [0]
listA_y = [0]
listAcc_E =[0]
listAcc_N =[0]
listG = []
listH = [np.pi/2]
listV_E = [0]
listV_N = [0]
listD_E = [1]
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
    delta_t = 0.1
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
        
        vel_E = listV_E[index - 1] + (accele_E + listAcc_E[index-1])/2 * 9.8 *delta_t #update velocity
        vel_N = listV_N[index - 1] + (accele_N + listAcc_N[index-1])/2 * 9.8 *delta_t
        listV_E.append(vel_E)
        listV_N.append(vel_N)

        dis_E = listD_E[index - 1] + (vel_E+listV_E[index-1])/2 * delta_t  #update displacement
        dis_N = listD_N[index - 1] + (vel_N+listV_N[index-1])/2 * delta_t
        listD_E.append(dis_E)
        listD_N.append(dis_N)
        
        global index
        index = index + 1
    elif(str_line[0] == 'T'):
        if(int(str_line[1:-2]) == 70):
            with open('data.csv','w') as mycsv:
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
            print("done output")
    else:
        print("err message")
    
while True:
    if(ser.in_waiting >0):
        line = ser.readline()
        if(line != b''):
            print(line)
            process_imu_data(line)
            
        
        

