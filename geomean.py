#!/usr/bin/python3
# Get geomean of two performance outputs
#
# @author Pavel Kryukov <kryukov@frtk.ru>
#
# For Putty AES NI project
# http://github.io/pavelkryukov/putty-aes-ni
import sys
import math

def average(iterable):
    return sum(iterable) / len(iterable)

def geomean(iterable):
    return math.exp(average([math.log(i) for i in iterable]))

array1 = [x.split() for x in open(sys.argv[1])]
array2 = [x.split() for x in open(sys.argv[2])]

if len(array1) != len(array2):
    raise "Array size mismatch!"

ratios = []
for row in range(len(array1)):
    for index in range(0, 2):
        if array1[row][index] != array2[row][index]:
            raise "Array content mismatch"
    ratios.append(float(array1[row][3]) / float(array2[row][3]))
    
print (str(geomean(ratios)) + " times")
