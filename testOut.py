<<<<<<< HEAD
import csv
import os
import time
import numpy as np
import pandas as pd
outputName = 'data0.csv'
start = time.time()
listA = list(range(1,1000000，0.1))
while(os.path.isfile(outputName)):
    print("while")
    dotPos = outputName.index('.')
    outputName = outputName.replace(outputName[4:dotPos], str(int(outputName[4:dotPos])+1))
with open(outputName,'w') as mycsv:
    wr = csv.writer(mycsv, quoting = csv.QUOTE_ALL)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)

print(time.time()-start)
=======
import csv
import os
import time
import numpy as np
import pandas as pd
outputName = 'data0.csv'
start = time.time()
arr = np.array(list(range(1,1000000)))
listA = list(range(1,1000000))
while(os.path.isfile(outputName)):
    print("while")
    dotPos = outputName.index('.')
    outputName = outputName.replace(outputName[4:dotPos], str(int(outputName[4:dotPos])+1))
with open(outputName,'w') as mycsv:
    wr = csv.writer(mycsv, quoting = csv.QUOTE_ALL)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)
    wr.writerow(listA)

print(start-time.time())
>>>>>>> e769ca10bf42efe864e87ed328c96a54adb57e5c
