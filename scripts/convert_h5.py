import sys
import os

import h5py
import numpy as np

from utils import read_data

POLICIES = ['OCCUPANCY', 'LABEL', 'COLOR']
POLICY = 'OCCUPANCY'

def convert_h5(voxels, output_filename):

    assert POLICY in POLICIES

    if POLICY == 'OCCUPANCY':
        # Take only z, y, x channel, and add 1 channel for occupany
        output_voxel = np.zeros((1,) + voxels.shape[:3], dtype=np.uint8)
       
        for z in range(voxels.shape[0]):
            for y in range(voxels.shape[1]):
                for x in range(voxels.shape[2]):
                    color = voxels[z][y][x]
                    if color[0] != 0 and color[1] != 0 and color[2] != 0:
                        output_voxel[0][z][y][x] = 1

        print(output_voxel.shape)
        print(np.mean(output_voxel))

        with h5py.File(output_filename, 'w') as f:
            f.create_dataset('data', data=output_voxel, compression='gzip')

def main():
    assert len(sys.argv) == 3
    color_voxel = read_data(sys.argv[1])
    print(color_voxel.shape)
    convert_h5(color_voxel, sys.argv[2])

if __name__ == '__main__':
    main()
