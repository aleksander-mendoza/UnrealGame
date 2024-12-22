#include "proc_assets.h"
#include "noise.h"

namespace proc_assets {
	void perlin(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float height, double uvScale, bool genTriangles) {
        planeScaledUVs(offset, resX, resY, size, out, [scale, height](FVector& vertex) {return noise::perlin_noise_derivative(float2(double2(vertex)), scale) * height; }, uvScale, genTriangles);
	}
    void perlin_fbm(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float heightPowerBase, const float scalePowerBase, int iterations, const float height, double uvScale, bool genTriangles) {
        planeScaledUVs(offset, resX, resY, size, out, [scale, height, heightPowerBase, scalePowerBase, iterations](FVector& vertex) {return noise::perlin_fbm_derivative(float2(double2(vertex)), scale, height, heightPowerBase, scalePowerBase, iterations); }, uvScale, genTriangles);
    }
	void morenoise(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float pointiness, const float scalingPowerBase, const int iterations, const float height, double uvScale, bool genTriangles) {
        planeScaledUVs(offset, resX, resY, size, out, [scale, height, pointiness, scalingPowerBase, iterations](FVector& vertex) {return noise::morenoise(float2(double2(vertex)), scale, pointiness, scalingPowerBase, iterations) * height; }, uvScale, genTriangles);
	}
    void cube(double3 offset, int resX, int resY, int resZ, double3 size, Mesh& out) {
    }

	void cube_without_uvs(double3 offset, double3 size, Mesh& out) {
        const double3 vertices[8] =
        {
            offset+double3(-size.X,  -size.Y,   -size.Z),
            offset+double3(size.X,   -size.Y,   -size.Z),
            offset+double3(size.X,   size.Y,   -size.Z),
            offset+double3(-size.X,  size.Y,   -size.Z),
            offset+double3(-size.X,  -size.Y,   size.Z),
            offset+double3(size.X,   -size.Y,   size.Z),
            offset+double3(size.X,   size.Y,   size.Z),
            offset+double3(-size.X,  size.Y,   size.Z),
        };

        double n = sqrt(3);
        const double3 normals[8] =
        {
            double3(-n,  -n,   -n),
            double3(n,   -n,   -n),
            double3(n,   n,   -n),
            double3(-n,  n,   -n),
            double3(-n,  -n,   n),
            double3(n,   -n,   n),
            double3(n,   n,   n),
            double3(-n,  n,   n),
        };
        const FProcMeshTangent tangents[8] =
        {
            FProcMeshTangent(-n,  -n,   n),
            FProcMeshTangent(n,   -n,   n),
            FProcMeshTangent(n,   n,   n),
            FProcMeshTangent(-n,  n,   n),
            FProcMeshTangent(-n,  -n,   -n),
            FProcMeshTangent(n,   -n,   -n),
            FProcMeshTangent(n,   n,   -n),
            FProcMeshTangent(-n,  n,   -n),
        };
        const FVector2D uvs[8] =
        {
            FVector2D(0,0),
            FVector2D(1,0),
            FVector2D(1,1),
            FVector2D(0,1),
            FVector2D(0,0),
            FVector2D(1,0),
            FVector2D(1,1),
            FVector2D(0,1),
        };              
        const int indices[6 * 6] =
        {
            0, 1, 3, 3, 1, 2,
            1, 5, 2, 2, 5, 6,
            5, 4, 6, 6, 4, 7,
            4, 0, 7, 7, 0, 3,
            3, 2, 7, 7, 2, 6,
            4, 5, 0, 0, 5, 1
        };

        out.vertices.Append(vertices, 8);
        out.triangles.Append(indices, 6*6);
        out.normals.Append(normals, 8);
        out.tangents.Append(tangents, 8);
        out.uvs.Append(uvs,8);


	}
    void Mesh::clearEverythingButTriangles()
    {
        vertices.Empty();
        uvs.Empty();
        normals.Empty();
        tangents.Empty();  
    }
    void Mesh::clear() {
        clearEverythingButTriangles();
        triangles.Empty();
        hasTriangles = false;
    }
}