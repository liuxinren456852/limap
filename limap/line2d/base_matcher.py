import os, sys
import numpy as np
from tqdm import tqdm
import joblib

sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
import limap.util.io as limapio

class BaseMatcher():
    def __init__(self, extractor, topk=10, n_neighbors=20, n_jobs=1):
        self.extractor = extractor
        self.topk = topk
        self.n_neighbors = n_neighbors
        self.n_jobs = n_jobs

    # The functions below are required for matchers
    def get_module_name(self):
        raise NotImplementedError
    def match_pair(self, descinfo1, descinfo2):
        raise NotImplementedError

    def get_matches_folder(self, output_folder):
        return os.path.join(output_folder, "{0}_n{1}_top{2}".format(self.get_module_name(), self.n_neighbors, self.topk))
    def read_descinfo(self, descinfo_folder, idx):
        return self.extractor.read_descinfo(descinfo_folder, idx)
    def get_match_filename(self, matches_folder, idx):
        fname = os.path.join(matches_folder, "matches_{0}.npy".format(idx))
        return fname
    def save_match(self, matches_folder, idx, matches):
        fname = self.get_match_filename(matches_folder, idx)
        limapio.save_npy(fname, matches)
    def read_match(self, matches_folder, idx):
        fname = self.get_match_filename(matches_folder, idx)
        return limapio.read_npy(fname)

    # TODO: multiprocessing does not give linear speed up for now!!
    def match_all_neighbors(self, output_folder, neighbors, descinfo_folder, skip_exists=False):
        matches_folder = self.get_matches_folder(output_folder)
        if not skip_exists:
            limapio.delete_folder(matches_folder)
        limapio.check_makedirs(matches_folder)
        n_images = len(neighbors)

        # multiprocessing unit
        def process(self, matches_folder, descinfo_folder, img_id, ng_img_id_list, skip_exists):
            fname_save = self.get_match_filename(matches_folder, img_id)
            if skip_exists and os.path.exists(fname_save):
                return
            descinfo1 = self.read_descinfo(descinfo_folder, img_id)
            matches_idx = []
            for ng_img_id in ng_img_id_list:
                descinfo2 = self.read_descinfo(descinfo_folder, ng_img_id)
                matches = self.match_pair(descinfo1, descinfo2)
                matches_idx.append(matches)
            self.save_match(matches_folder, img_id, matches_idx)
        joblib.Parallel(n_jobs=self.n_jobs)(joblib.delayed(process)(self, matches_folder, descinfo_folder, img_id, neighbors[img_id], skip_exists) for img_id in tqdm(range(n_images)))
        return matches_folder

