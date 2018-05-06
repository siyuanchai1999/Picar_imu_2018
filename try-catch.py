import sys
try:
    z = 5/0
except Exception as e:
    
    with open('error.txt','w')as log:
        log.write('ERROR LOG\n')
        log.write(str(e))
