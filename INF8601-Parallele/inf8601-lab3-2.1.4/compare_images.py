#! /usr/bin/python

import numpy as np
from PIL import Image
import sys

test_image = 'reference-medium-2x2-2000.png'

image = raw_input('Enter your image name : ')

img1 = Image.open(test_image).convert('RGB')
img2 = Image.open(image).convert('RGB')

vect1 = map(int, np.array(img1).ravel())
vect2 = map(int, np.array(img2).ravel())

if(vect1.__len__() != vect2.__len__()):
    print("Images are not of the same size")
    sys.exit(0)

diff = np.uint64(0)

for i in range(0,vect1.__len__()):
    diff += abs(vect1[i] - vect2[i])
    
print("Total difference is : " + str(diff))
rDiff = 100*diff/vect1.__len__()
print("Relative difference is : " + str(rDiff) + " %")
