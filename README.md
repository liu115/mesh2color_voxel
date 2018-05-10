# Convert Mesh to Color Voxel

Take `.ply` mesh format as input, convert it into voxel with color (rgba).

## Requirement

- c++11: for the package **tinyply**.
- numpy

## Usage

### Take the largest face inside the cube (voxel) to represent to color.
```
make MAX
./mesh2vox_max [input ply file] // output file is .dat
```
### Take weighted mean (by area size of faces) for the color of the voxel.
```
make MEAN
./mesh2vox_mean [input ply file] // output file is .dat
```
### Visualization

```
python3 read_buffer.py // generate .ply point cloud for visualization
```
Use tools like meshlab to open the `.ply` file.

## References

- tinyply: https://github.com/ddiakopoulos/tinyply
- 3d polygon area: http://geomalgorithms.com/a01-_area.html
- triangle test: http://blackpawn.com/texts/pointinpoly/
- triangle cube intersect test: http://www.realtimerendering.com/resources/GraphicsGems/gemsiii/triangleCube.c

