import serial
import csv
ser = serial.Serial('/dev/ttyACM0',9600)
 

listA_x = []
listA_y = []
listG = []
listH = []

sumA_x = 0.0
sumA_y = 0.0
sumG = 0.0
sumH = 0.0
 
def process_imu_data(line): 
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
        listA_y.append(A_x)
    elif(str_line[0] == 'G'):
        G = float(str_line[1:-2])
        global sumG
        sumG = sumG + G
        listG.append(G)
    elif(str_line[0] == 'H'):
        H = float(str_line[1:-2])
        global sumH
        sumH = sumH + H
        listH.append(H)
    elif(str_line[0] == 'T'):
        if(int(str_line[1:-2]) == 70):
            with open('data.csv','w') as mycsv:
                wr = csv.writer(mycsv, quoting = csv.QUOTE_ALL)
                listA_x.append(sumA_x)
                wr.writerow(listA_x)
                listA_x.pop()
                 
                listA_y.append(sumA_y)
                wr.writerow(listA_y)
                listA_y.pop()
                
                listG.append(sumG)
                wr.writerow(listG)
                listG.pop()
                
                listH.append(sumH)
                wr.writerow(listH)
                listH.pop()
            print("done output")
    else:
        print("err message")
    
while True:
    if(ser.in_waiting >0):
        line = ser.readline()
        if(line != b''):
            print(line)
            process_imu_data(line)
            
        
        
