#!/usr/bin/python3
"""Compare two aligned images of the same size.
Usage: python compare.py first-image second-image
"""

import sys

from scipy.misc import imread
from scipy.linalg import norm
from scipy import sum, average
import logging
import os
import datetime

class ManhattanCompare:
    def __init__(self):
        pass
    
    def compare(self, file1, file2):
        # read images as 2D arrays (convert to grayscale for simplicity)
        img1 = self.to_grayscale(imread(file1).astype(float))
        img2 = self.to_grayscale(imread(file2).astype(float))
        # compare
        n_m, n_0 = self.compare_images(img1, img2)
        logging.debug("Manhattan norm:" + str(n_m) + "/ per pixel:" + str(n_m/img1.size))
        logging.debug("Zero norm:" + str(n_0) + "/ per pixel:" + str(n_0*1.0/img1.size))

    def compare_images(self, img1, img2):
        # normalize to compensate for exposure difference
        img1 = self.normalize(img1)
        img2 = self.normalize(img2)
        # calculate the difference and its norms
        diff = img1 - img2  # elementwise for scipy arrays
        m_norm = sum(abs(diff))  # Manhattan norm
        z_norm = norm(diff.ravel(), 0)  # Zero norm
        return (m_norm, z_norm)

    def to_grayscale(self, arr):
        "If arr is a color image (3D array), convert it to grayscale (2D array)."
        if len(arr.shape) == 3:
            return average(arr, -1)  # average over the last axis (color channels)
        else:
            return arr

    def normalize(self, arr):
        rng = arr.max()-arr.min()
        amin = arr.min()
        return (arr-amin)*255/rng

def compare(i,j):
    file1 = "./images/" + str(i) + ".jpg"
    file2 = "./images/" + str(j) + ".jpg"
    mc = ManhattanCompare()
    logging.debug("Comparing " + file1 + " with " + file2)
    mc.compare(file1,file2)

if __name__ == "__main__":
    #logFileName = './logs/{0:%Y-%m-%d_%H:%M:%S}.log'.format(datetime.datetime.now())
    logFileName = './logs/manhattan.log.txt'.format(datetime.datetime.now())
    if os.path.exists(logFileName):
        os.remove(logFileName)
    logging.basicConfig(filename=logFileName,level=logging.DEBUG)
    file1 = "./images/1.jpg"
    file2 = "./images/2.jpg"
    compare(1,2)
    compare(1,5)
    compare(2,5)
    
