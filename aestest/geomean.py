#!/usr/bin/python
# Get geomean of two performance outputs
#
# @author Pavel Kryukov <kryukov@frtk.ru>
#
# For Putty AES NI project
# http://github.io/pavelkryukov/putty-aes-ni
import sys
import numpy as np

def geomean(iterable):
    a = np.log(iterable)
    return np.exp(a.sum()/len(a))

array1 = np.loadtxt(sys.argv[1], skiprows=1)
array2 = np.loadtxt(sys.argv[2], skiprows=1)

if len(array1) != len(array2):
    raise "Array size mismatch!"

ratios = []
for row in range(len(array1)):
    for index in range(0, 2):
        if array1[row][index] != array2[row][index]:
            raise "Array content mismatch"
    ratios.append(float(array1[row][3]) / float(array2[row][3]))
    
print str(geomean(ratios)) + " times"
