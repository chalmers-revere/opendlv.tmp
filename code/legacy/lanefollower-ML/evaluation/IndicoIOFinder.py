import math
import os
from random import sample
import pickle

from scipy import spatial
from PIL import Image
import numpy as np
import indicoio

# number of images to compare with input for match
N_IMG = 18

class IndicoIOFinder:
    """Finds similar images using the indicoIO API for machine learning
       https://indico.io/blog/clothing-similarity-how-a-program-is-more-fashionable-than-me/
       https://pypi.python.org/pypi/IndicoIo
    """
    def __init__(self,path):
        self.userpath = path
        pass
    
    def is_image(self,filename):
        f = filename.lower()
        return f.endswith(".png") or f.endswith(".jpg") or f.endswith(".jpeg") or f.endswith(".bmp") or f.endswith(".gif")
    
    def make_paths_list(self):
        return [os.path.join(self.userpath, path) for path in os.listdir(self.userpath) if self.is_image(path)]
    
    def make_feats(self,paths):
        return indicoio.image_features(paths, batch=True, api_key='e7f946ec6ffa6d59e6e9e3297941cc6a')
        #return []

    def calculate_sim(self,feats):
        m = []
        if len(feats) > 0:
            distances = spatial.distance.pdist(np.matrix(feats), 'euclidean')
            q = lambda i,j,n: n*(n-1)/2 - (n-i)*(n-i-1)/2 + j - i - 1
            for i in range(N_IMG):
                r = []
                for j in range(N_IMG):
                    tup = (distances[q(i,j,N_IMG)], j)
                    if i == j:
                        tup = (0, j)
                    r.append(tup)
                r = sorted(r, key=lambda x: x[0])
                m.append(r)
        return m

    def similarity_image(self,chosen_img, similarity_matrix, paths):
        if len(similarity_matrix) is 0:
            return
        new_img = Image.new('RGB', (640, 480), "#f8fafc")
        for i in range(10):
            im_num = similarity_matrix[chosen_img][i][1]
            path = paths[im_num]
            img = Image.open(path)
            img.thumbnail((200, 200))
            pos = ((i % 5) * 210, int(math.floor(i / 5.0) * 210))
            new_img.paste(img, pos)
            new_img.save('../testdata/'+ str(N_IMG) + 'if' + str(chosen_img) + '.jpg')
        new_img.show()

    def findInDirectory(self):
        paths = self.make_paths_list()
        #print("paths = " + str(paths))
        feats = self.make_feats(paths)
        print("feats = " + str(feats))
        similarity_rankings = self.calculate_sim(feats)
        print("similarity_rankings = " + str(similarity_rankings))
        chosen_images = sample(range(N_IMG), 3)
        for k in range(len(chosen_images)):
            chosen_img = chosen_images[k]
            self.similarity_image(chosen_img, similarity_rankings, paths)

if __name__ == '__main__':
    import sys, os
    def usage():
        sys.stderr.write("SYNOPSIS: %s [<directory>]\n" % sys.argv[0])
        sys.exit(1)
    # Get path
    path = sys.argv[1] if len(sys.argv) > 1 else usage()
    # Go
    iiof = IndicoIOFinder(path)
    iiof.findInDirectory()
