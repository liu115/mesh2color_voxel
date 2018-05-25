#include "triangleCube.h"
#include "tinyply.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <assert.h>
using namespace tinyply;
using namespace std;

// #define TAKE_MAX_AREA_COLOR
#define AREA_THRESHOLD 1e-5
#define MAX_VOX_X 200
#define MAX_VOX_Y 200
#define MAX_VOX_Z 150
#define CUBESIZE 48 // 48 mm
typedef struct int3 {
    int3() {}
    int3(int a, int b, int c) {
        v[0] = a, v[1] = b, v[2] = c;
    }
    int v[3];
} int3;

typedef struct int4 {
    int4() {}
    int4(int a, int b, int c) {
        v[0] = a, v[1] = b, v[2] = c, v[3] = 255;
    }
    int4(int a, int b, int c, int d) {
        v[0] = a, v[1] = b, v[2] = c, v[3] = d;
    }
    int v[4];
} int4;

typedef struct {
    unsigned char v[4];
} uchar4;


typedef enum {
    B_MAX, B_MIN
} boundary_type;

// Debug
void print_point3(Point3 p) {
    cout << '(' << p.x << ", " << p.y << ", " << p.z << ')' << endl;
}

void print_triangle3(Triangle3 &tri) {
    cout << "v1: ";
    print_point3(tri.v1);
    cout << "v2: ";
    print_point3(tri.v2);
    cout << "v3: ";
    print_point3(tri.v3);
}
// Set maximum size
int4 global_voxel[MAX_VOX_X][MAX_VOX_Y][MAX_VOX_Z];
float voxel_buffer[MAX_VOX_X][MAX_VOX_Y][MAX_VOX_Z][5];
Point3 triangle_boundary(Triangle3 &tri, boundary_type type) {
    Point3 p;
    
    if (type == B_MAX) {
        p.x = MAX3(tri.v1.x, tri.v2.x, tri.v3.x);
        p.y = MAX3(tri.v1.y, tri.v2.y, tri.v3.y);
        p.z = MAX3(tri.v1.z, tri.v2.z, tri.v3.z);
    }
    else if (type == B_MIN) {
        p.x = MIN3(tri.v1.x, tri.v2.x, tri.v3.x);
        p.y = MIN3(tri.v1.y, tri.v2.y, tri.v3.y);
        p.z = MIN3(tri.v1.z, tri.v2.z, tri.v3.z);
    }
    return p;
}
int3 point3_to_voxel_cord(Point3 p) {
    int3 voxel_cord;
    voxel_cord.v[0] = (int) (p.x / CUBESIZE);
    voxel_cord.v[1] = (int) (p.y / CUBESIZE);
    voxel_cord.v[2] = (int) (p.z / CUBESIZE);
    return voxel_cord;
}

Point3 point3_scale(Point3 p, float ratio) {
    Point3 new_p;
    new_p.x = p.x * ratio;
    new_p.y = p.y * ratio;
    new_p.z = p.z * ratio;
    return new_p;
}

void triangle_cord_transform(Triangle3 &tri, float scale, Point3 trans) {
    // Firest translate, then resize
    ADD(tri.v1, trans, tri.v1);
    ADD(tri.v2, trans, tri.v2);
    ADD(tri.v3, trans, tri.v3);

    tri.v1 = point3_scale(tri.v1, scale);
    tri.v2 = point3_scale(tri.v2, scale);
    tri.v3 = point3_scale(tri.v3, scale);
}
Point3 point_float_mult(Point3 p, float a) {
    Point3 t(p.x*a, p.y*a, p.z*a);
    return t;
}

void mark_occupy_voxel(Triangle3 &tri, int4 &color) {
    Point3 tri_bound_max = triangle_boundary(tri, B_MAX);
    Point3 tri_bound_min = triangle_boundary(tri, B_MIN);
    int3 vox_bound_max = point3_to_voxel_cord(tri_bound_max);
    int3 vox_bound_min = point3_to_voxel_cord(tri_bound_min);
    
    // Clip the min boundary at 0. (Voxels less than 0 will not be considered.) 
    int vox_min_x = max(vox_bound_min.v[0], 0);
    int vox_min_y = max(vox_bound_min.v[1], 0);
    int vox_min_z = max(vox_bound_min.v[2], 0);
    for (int i = vox_min_x; i <= vox_bound_max.v[0]; i++) {
        for (int j = vox_min_y; j <= vox_bound_max.v[1]; j++) {
            for (int k = vox_min_z; k <= vox_bound_max.v[2]; k++) {
                // Calculate difference between voxel and unit cube
                float scale = 1. / CUBESIZE;
                Point3 unit_cube_centroid(0, 0, 0);
                Point3 target_cube_centroid((i+.5)*CUBESIZE, (j+.5)*CUBESIZE, (k+.5)*CUBESIZE);
                Point3 trans;
                SUB(unit_cube_centroid, target_cube_centroid, trans);
                Triangle3 tmp_tri = tri;
                // Transform triangle into unit cube coordinate
                triangle_cord_transform(tmp_tri, scale, trans);

                // DO occupy check
                if (t_c_intersection(tmp_tri) == INSIDE) {
                    float area = triangle_area_inside_cube(tmp_tri);
                    // CHECK Threshold
                    if (area < AREA_THRESHOLD) continue;
#ifdef TAKE_MAX_AREA_COLOR
                    if (area > voxel_buffer[i][j][k][4]) {
                        voxel_buffer[i][j][k][0] = color.v[0];
                        voxel_buffer[i][j][k][1] = color.v[1];
                        voxel_buffer[i][j][k][2] = color.v[2];
                        voxel_buffer[i][j][k][3] = color.v[3];
                        
                    }
#else 
                    // Save for weighted mean
                    voxel_buffer[i][j][k][0] += color.v[0] * area;
                    voxel_buffer[i][j][k][1] += color.v[1] * area;
                    voxel_buffer[i][j][k][2] += color.v[2] * area;
                    voxel_buffer[i][j][k][3] += color.v[3] * area;
                    voxel_buffer[i][j][k][4] += area;
#endif
                }
            }
        }
    }
}

void dump_voxel(int4 voxel[MAX_VOX_X][MAX_VOX_Y][MAX_VOX_Z], int3 dim, const char fname[], const char scene[]){
    int hdim = dim.v[0];
    int wdim = dim.v[1];
    int ddim = dim.v[2];
    int cdim = 4;
    ofstream fout(fname, ios::out | ios::binary);
    fout << "CV\n";
    fout << "# "<< scene<< "color voxel. Height, width, depth, channel\n";
    fout << hdim <<" "<< wdim <<" "<< ddim <<" "<< cdim <<"\n";
    fout << hdim*wdim*ddim*cdim<<"\n";
    for(int h = 0; h < hdim; ++h){
        for(int w = 0; w < wdim; ++w){
            for(int d = 0; d < ddim; ++d){
                fout.write(reinterpret_cast<const char*>(&voxel[h][w][d].v[0]), sizeof(int));
                fout.write(reinterpret_cast<const char*>(&voxel[h][w][d].v[1]), sizeof(int));
                fout.write(reinterpret_cast<const char*>(&voxel[h][w][d].v[2]), sizeof(int));
                fout.write(reinterpret_cast<const char*>(&voxel[h][w][d].v[3]), sizeof(int));
            }
        }
    }
    fout.close();
}

void ply_to_voxel(const string &filename) {
    try
    {
        // Read the file and create a std::istringstream suitable
        // for the lib -- tinyply does not perform any file i/o.
        std::ifstream ss(filename, std::ios::binary);

        if (ss.fail())
        {
            throw std::runtime_error("failed to open " + filename);
        }

        // Read ply file
        PlyFile file;
        shared_ptr<PlyData> faces, vertices, colors;
        file.parse_header(ss);
        faces = file.request_properties_from_element("face", { "vertex_indices" });
        vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
        colors = file.request_properties_from_element("vertex", {"red", "green", "blue", "alpha"});
        
        file.read(ss);

        // Copy data from plyfile reader to vectors
        const size_t numVerticesBytes = vertices->buffer.size_bytes();
        const size_t numFacesBytes = faces->buffer.size_bytes();
        const size_t numColorBytes = colors->buffer.size_bytes();
        vector<Point3> v(vertices->count);
        vector<int3> f(faces->count);
        vector<uchar4> v_color(vertices->count);
        memcpy(v.data(), vertices->buffer.get(), numVerticesBytes);
        memcpy(f.data(), faces->buffer.get(), numFacesBytes);
        memcpy(v_color.data(), colors->buffer.get(), numColorBytes);
        // Find the boundary of the mesh
        Point3 max_boundary = v[0], min_boundary = v[0];
        for (int i = 0; i < v.size(); i++) {
            v[i].x *= 1e3;
            v[i].y *= 1e3;
            v[i].z *= 1e3;

            max_boundary.x = max(max_boundary.x, v[i].x);
            max_boundary.y = max(max_boundary.y, v[i].y);
            max_boundary.z = max(max_boundary.z, v[i].z);
            min_boundary.x = min(min_boundary.x, v[i].x);
            min_boundary.y = min(min_boundary.y, v[i].y);
            min_boundary.z = min(min_boundary.z, v[i].z);
        }
        // Only consider min_boundary is (0, 0, 0)
        const int max_vox_x = (int)(max_boundary.x / CUBESIZE) + 1;
        const int max_vox_y = (int)(max_boundary.y / CUBESIZE) + 1;
        const int max_vox_z = (int)(max_boundary.z / CUBESIZE) + 1;
        cout << max_vox_x  << ' ' << max_vox_y << ' ' << max_vox_z << endl;

        // Reset globel voxel
        memset(global_voxel, 0, sizeof(global_voxel));
        for (int i = 0; i < MAX_VOX_X; i++)
            for (int j = 0; j < MAX_VOX_Y; j++)
                for (int k = 0; k < MAX_VOX_Z; k++)
                    for (int l = 0; l < 5; l++)
                        voxel_buffer[i][j][k][l] = 0;
        
        // Check if it is larger than max size
        assert(max_vox_x < MAX_VOX_X);
        assert(max_vox_y < MAX_VOX_Y);
        assert(max_vox_z < MAX_VOX_Z);

        // Setup all trangles from ids to real points
        vector<Triangle3> tri(faces->count);
        vector<int4> tri_color(faces->count);
        for (int i = 0; i < faces->count; i++) {
            tri[i].v1 = v[f[i].v[0]];
            tri[i].v2 = v[f[i].v[1]];
            tri[i].v3 = v[f[i].v[2]];
            
            // Calc color for the face
            tri_color[i].v[0] = (v_color[f[i].v[0]].v[0] + v_color[f[i].v[1]].v[0] + v_color[f[i].v[2]].v[0]) / 3;
            tri_color[i].v[1] = (v_color[f[i].v[0]].v[1] + v_color[f[i].v[1]].v[1] + v_color[f[i].v[2]].v[1]) / 3;
            tri_color[i].v[2] = (v_color[f[i].v[0]].v[2] + v_color[f[i].v[1]].v[2] + v_color[f[i].v[2]].v[2]) / 3;
            tri_color[i].v[3] = (v_color[f[i].v[0]].v[3] + v_color[f[i].v[1]].v[3] + v_color[f[i].v[2]].v[3]) / 3;
            // Do occupying check
            mark_occupy_voxel(tri[i], tri_color[i]);
        }

        // Weighted mean
        for (int i = 0; i < MAX_VOX_X; i++)
            for (int j = 0; j < MAX_VOX_Y; j++)
                for (int k = 0; k < MAX_VOX_Z; k++) {
#ifdef TAKE_MAX_AREA_COLOR
                    
                    global_voxel[i][j][k].v[0] = voxel_buffer[i][j][k][0]; 
                    global_voxel[i][j][k].v[1] = voxel_buffer[i][j][k][1]; 
                    global_voxel[i][j][k].v[2] = voxel_buffer[i][j][k][2]; 
                    global_voxel[i][j][k].v[3] = voxel_buffer[i][j][k][3]; 
#else
                    // Weighted mean
                    if (voxel_buffer[i][j][k][4] < AREA_THRESHOLD) continue;
                    global_voxel[i][j][k].v[0] = voxel_buffer[i][j][k][0] / voxel_buffer[i][j][k][4]; 
                    global_voxel[i][j][k].v[1] = voxel_buffer[i][j][k][1] / voxel_buffer[i][j][k][4]; 
                    global_voxel[i][j][k].v[2] = voxel_buffer[i][j][k][2] / voxel_buffer[i][j][k][4]; 
                    global_voxel[i][j][k].v[3] = voxel_buffer[i][j][k][3] / voxel_buffer[i][j][k][4]; 
                    // printf("area: %f\n", voxel_buffer[i][j][k][4]); 
#endif
                }

        // Write to file
        int3 dim(max_vox_x, max_vox_y, max_vox_z);
        dump_voxel(global_voxel, dim, "sceneXXXX_XX.dat", "sceneXXXX_XX");
	}
    catch (const std::exception & e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }    
}

int main(int argc, char *argv[]) {
#ifdef TAKE_MAX_AREA_COLOR
    printf("Using max area for color\n");
#else
    printf("Using weighted mean for color\n");
#endif
    printf("Reading file %s\n", argv[1]);
    ply_to_voxel(argv[1]);
    return 0;
}
