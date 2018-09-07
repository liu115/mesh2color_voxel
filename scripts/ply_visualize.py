import sys
import numpy as np

from utils import read_data

def colorvol2points(voxels):
    points = []
    rgb = []
    for z in range(voxels.shape[0]):
        for y in range(voxels.shape[1]):
            for x in range(voxels.shape[2]):
                color = voxels[z][y][x]
                if color[0] != 0 and color[1] != 0 and color[2] != 0:
                    points.append(np.array([x, y, z]))
                    rgb.append(color)
    points = np.vstack(points)
    rgb = np.vstack(rgb)
    return {'points': points, 'rgb': rgb}


def writeply(filename, points, rgb):
    target = open(filename, 'w')

    # write the  header
    target.write('ply\n')
    target.write('format ascii 1.0 \n')
    target.write('element vertex ' + str(points.shape[0]) + '\n')
    target.write('property float x\n')
    target.write('property float y\n')
    target.write('property float z\n')
    target.write('property uchar red\n')
    target.write('property uchar green\n')
    target.write('property uchar blue\n')
    target.write('end_header\n')

    # write the  points
    for i in range(points.shape[0]):
        target.write('%f %f %f %d %d %d\n' % (
            points[i, 0], points[i, 1], points[i, 2], rgb[i, 0], rgb[i, 1], rgb[i, 2]))
    target.close()


def color_voxel_label_to_ply(voxel, output_ply):
    points = colorvol2points(voxel)
    writeply(output_ply, points['points'], points['rgb'])
    print('output: ' + output_ply)


def main():
    assert len(sys.argv) == 2
    color_voxel = read_data(sys.argv[1])
    print(color_voxel)
    color_voxel_label_to_ply(color_voxel, "output.ply")

if __name__ == '__main__':
    main()