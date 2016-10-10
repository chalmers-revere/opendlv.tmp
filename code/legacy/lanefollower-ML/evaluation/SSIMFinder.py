#!/usr/bin/env python

import sys, os
from ssim.ssimlib import SSIM
from ssim.utils import get_gaussian_kernel
from Loggable import Loggable
from operator import itemgetter

class SSIMFinder(Loggable):
   
    def __init__(self,logger):
        Loggable.__init__(self,logger,SSIMFinder.__name__)
        # Prepare magic values
        gaussian_kernel_sigma = 1.5
        gaussian_kernel_width = 11
        self.gaussian_kernel_1d = get_gaussian_kernel(gaussian_kernel_width, gaussian_kernel_sigma)
    
    def is_image(self,filename):
        f = filename.lower()
        return f.endswith(".png") or f.endswith(".jpg") or f.endswith(".jpeg") or f.endswith(".bmp") or f.endswith(".gif")
    
    def findAllSimilarInDirectory(self, userpath, treshold):
        image_filenames = [os.path.join(userpath, path) for path in os.listdir(userpath) if self.is_image(path)]
        #print("images = " + str(image_filenames))
        for base_img in sorted(image_filenames):
            print("base_img = " + base_img)
            self.ssim = SSIM(base_img,self.gaussian_kernel_1d)
            for img in sorted(image_filenames):
                ssim_value = self.ssim.ssim_value(img)
                #print("ssim = " + str(ssim_value))
                if (base_img is not img) and (ssim_value > treshold):
                    print("  is similar to " + img + "(" + str(ssim_value) + ")")
    
    def findMostSimilarInDirectory(self, image_path, directory, numImages=10, treshold=0.5):
        self.info("Searching " + str(numImages) + " most similar images to " + image_path + " in directory " + directory + " with treshold " + str(treshold))
        image_filenames = [os.path.join(directory, path) for path in os.listdir(directory) if self.is_image(path)]
        self.ssim = SSIM(image_path, self.gaussian_kernel_1d)
        result = [] # A list of tuples (image file name, SSIM value)
        min_ssim = 1
        max_ssim = 0
        for img in sorted(image_filenames):
            ssim_value = self.ssim.ssim_value(img)
            if ssim_value > max_ssim:
                max_ssim = ssim_value
            if ssim_value < min_ssim:
                min_ssim = ssim_value
            if ssim_value >= treshold:
                self.debug("The image " + image_path + " is similar to " + img + " (" + str(ssim_value) + ")")
                data = (img,ssim_value)
                result.append(data)
        self.info("SSIM MIN = " + str(min_ssim) + ", MAX = " + str(max_ssim))
        sorted_result = sorted(result,key=itemgetter(1),reverse=True)
        self.debug("Similarity results: " + str(sorted_result))
        return sorted_result[:10]

if __name__ == '__main__':
    import sys, os
    def usage():
        sys.stderr.write("""SYNOPSIS: %s [<directory>]""" % sys.argv[0])
        sys.exit(1)
    # Get path
    path = sys.argv[1] if len(sys.argv) > 1 else "."
    # Go
    sf = SSIMFinder()
    sf.findAllSimilarInDirectory(path)
