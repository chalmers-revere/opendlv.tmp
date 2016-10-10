#!/usr/bin/env python
from PIL import Image
import imagehash
import os

"""
Demo of hashing
"""
class HashFinder:
    def __init__(self):
        pass
    
    def is_image(self,filename):
    	f = filename.lower()
    	return f.endswith(".png") or f.endswith(".jpg") or \
    		f.endswith(".jpeg") or f.endswith(".bmp") or f.endswith(".gif")
     
    def findUsingAverageHash(self, userpath, hash_size=8):
        image_filenames = [os.path.join(userpath, path) for path in os.listdir(userpath) if self.is_image(path)]
        images = {}
        for img in sorted(image_filenames):
        	hash = imagehash.average_hash(Image.open(img), hash_size)
        	images[hash] = images.get(hash, []) + [img]
        
        print("=== Average Hash Results ===")
        for k, img_list in images.items():
        	if len(img_list) > 1:
        		print(" ".join(img_list))
    
    def findUsingPerceptualHash(self, userpath, hash_size=8):
        image_filenames = [os.path.join(userpath, path) for path in os.listdir(userpath) if self.is_image(path)]
        images = {}
        for img in sorted(image_filenames):
        	hash = imagehash.phash(Image.open(img), hash_size)
        	images[hash] = images.get(hash, []) + [img]
        	
        print("=== Perceptual Hash Results ===")
        for k, img_list in images.items():
        	if len(img_list) > 1:
        		print(" ".join(img_list))
    
    def findUsingDifferenceHash(self, userpath, hash_size=8):
        image_filenames = [os.path.join(userpath, path) for path in os.listdir(userpath) if self.is_image(path)]
        images = {}
        for img in sorted(image_filenames):
        	hash = imagehash.dhash(Image.open(img), hash_size)
        	images[hash] = images.get(hash, []) + [img]
        	
        print("=== Difference Hash Results ===")
        for k, img_list in images.items():
        	if len(img_list) > 1:
        		print(" ".join(img_list)) 		
    
    def find_similar_images(self, userpath, hashfunc = imagehash.average_hash):
        image_filenames = [os.path.join(userpath, path) for path in os.listdir(userpath) if self.is_image(path)]
        images = {}
        for img in sorted(image_filenames):
        	hash = hashfunc(Image.open(img))
        	images[hash] = images.get(hash, []) + [img]
        
        for k, img_list in images.items():
        	if len(img_list) > 1:
        		print(" ".join(img_list))

if __name__ == '__main__':
    import sys, os
    def usage():
    	sys.stderr.write("""SYNOPSIS: %s [ahash|phash|dhash] [<directory>]

Identifies similar images in the directory.

Method: 
  ahash: Average hash
  phash: Perceptual hash
  dhash: Difference hash

(C) Johannes Buchner, 2013
""" % sys.argv[0])
    	sys.exit(1)
    
    hashmethod = sys.argv[1] if len(sys.argv) > 1 else usage()
    if hashmethod == 'ahash':
    	hashfunc = imagehash.average_hash
    elif hashmethod == 'phash':
    	hashfunc = imagehash.phash
    elif hashmethod == 'dhash':
    	hashfunc = imagehash.dhash
    else:
    	usage()
    userpath = sys.argv[2] if len(sys.argv) > 2 else "."
    sf = HashFinder()
    sf.find_similar_images(userpath=userpath, hashfunc=hashfunc)
    # Average Hashes
    sf.findUsingAverageHash(userpath, 2)
    sf.findUsingAverageHash(userpath, 4)
    sf.findUsingAverageHash(userpath, 50)
    sf.findUsingAverageHash(userpath, 100)
    sf.findUsingAverageHash(userpath, 200)
    # Perceptual Hashes
    sf.findUsingPerceptualHash(userpath, 2)
    sf.findUsingPerceptualHash(userpath, 4)
    sf.findUsingPerceptualHash(userpath, 50)
    sf.findUsingPerceptualHash(userpath, 100)
    sf.findUsingPerceptualHash(userpath, 200)
    # Difference Hashes
    sf.findUsingDifferenceHash(userpath, 2)
    sf.findUsingDifferenceHash(userpath, 4)
    sf.findUsingDifferenceHash(userpath, 50)
    sf.findUsingDifferenceHash(userpath, 100)
    sf.findUsingDifferenceHash(userpath, 200)
