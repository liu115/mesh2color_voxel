import numpy as np
import re

def read_data(filename, byteorder='>'):
    """Return image data from a raw PGM file as numpy array.

    Format specification: http://netpbm.sourceforge.net/doc/pgm.html

    """
    with open(filename, 'rb') as f:
        buffer = f.read()
    try:
        header, height, width, depth, channel, maxval = re.search(
            b"(^CV\s(?:\s*#.*[\r\n])*"
            b"(\d+)\s(?:\s*#.*[\r\n])*"
            b"(\d+)\s(?:\s*#.*[\r\n])*"
            b"(\d+)\s(?:\s*#.*[\r\n])*"
            b"(\d+)\s(?:\s*#.*[\r\n])*"
            b"(\d+)\s(?:\s*#.*[\r\n]\s)*)", buffer).groups()
    except AttributeError:
        raise ValueError("File header not found: '%s'" % filename)
    #print(height)
    #print(width)
    #print(depth)
    #print(channel)
    #print(maxval)
    color_voxel= np.frombuffer(buffer,
                         dtype=np.int32,
                         count=int(height)*int(width)*int(depth)*int(channel),
                         offset=len(header)
                         ).reshape((int(height), int(width), int(depth), int(channel)))
    #print(color_voxel.shape)
    return color_voxel