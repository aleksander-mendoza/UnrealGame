#pragma once
#include "CoreMinimal.h"
#include "utildefines.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"


namespace proc_assets {
	class Mesh {
	public:
		TArray<FVector> vertices;
		TArray<FVector2D> uvs;
		TArray<int32> triangles;
		TArray<FVector> normals;
		TArray<FProcMeshTangent> tangents;
		bool hasTriangles=false;
		Mesh() :vertices(), uvs(), triangles(), normals(), tangents(){}
		void clearEverythingButTriangles();
		void clear();
	};
	/**plane is just a horizontally aligned grid*/
	template<typename F>
	void plane(double3 offset, int resX, int resY, float2 size, Mesh& out, F heightFunction, bool genTriangles) {
		plane(offset, resX, resY, size, out, heightFunction, [resX, resY](int32 vx, int32 vy, double3 vertex) {return double2(vx / double(resX-1), vy / double(resY-1)); }, genTriangles);
	}
	template<typename F>
	void planeScaledUVs(double3 offset, int resX, int resY, float2 size, Mesh& out, F heightFunction, double uvScale, bool genTriangles) {
		plane(offset, resX, resY, size, out, heightFunction, [uvScale, resX, resY](int32 vx, int32 vy, double3 vertex) {return double2(vertex)* uvScale; }, genTriangles);
	}
	/**plane is just a horizontally aligned grid*/
	template<typename F, typename UV>
	void plane(double3 offset, int resX, int resY, float2 size, Mesh& out, F heightFunction, UV uvTransform, bool genTriangles) {
		grid(offset, resX, resY, double3(size.X, 0, 0), double3(0, size.Y, 0), double3(0, 0, 1), out, heightFunction, uvTransform, genTriangles);
	}

	template<typename F, typename UV>
	void grid(double3 offset, int resX, int resY, double3 width, double3  height, double3 displacementDirection, Mesh & out, F displacementFunction, UV uvTransform, bool genTriangles) {
		const double3 spacingX = width / (double)(resX - 1);
		const double3 spacingY = height / (double)(resY - 1);
		out.vertices.Reserve(out.vertices.Num() + resY * resX);
		out.uvs.Reserve(out.uvs.Num() + resY * resX);
		out.normals.Reserve(out.normals.Num() + resY * resX);
		out.tangents.Reserve(out.normals.Num() + resY * resX);

		for (int32 vy = 0; vy < resY; vy++) {
			for (int32 vx = 0; vx < resX; vx++) {
				FVector vertex = offset + double(vx) * spacingX + double(vy) * spacingY;
				float3 derivative_and_displacement = displacementFunction(vertex);

				vertex += displacementDirection * derivative_and_displacement.Z;
				out.vertices.Add(vertex);
				FVector2D uv(uvTransform(vx, vy, vertex));
				out.uvs.Add(uv);
				float3 normal = math::normalize(math::normal(float2(derivative_and_displacement)));
				out.normals.Add(FVector(normal));
				float3 tangent = math::normalize(math::tangent(float2(derivative_and_displacement)));
				out.tangents.Add(FProcMeshTangent(FVector(tangent), false));
			}
		}

		if (genTriangles && !out.hasTriangles) {
			out.triangles.Reserve(out.triangles.Num() + (resY - 1) * (resX - 1) * 2);
			for (int32 vy = 0; vy < resY - 1; vy++) {
				for (int32 vx = 0; vx < resX - 1; vx++) {
					const int bottomLeft = vx + vy * resX;
					const int bottomRight = bottomLeft + 1;
					const int topLeft = bottomLeft + resX;
					const int topRight = topLeft + 1;
					out.triangles.Add(bottomLeft);
					out.triangles.Add(topLeft);
					out.triangles.Add(bottomRight);

					out.triangles.Add(bottomRight);
					out.triangles.Add(topLeft);
					out.triangles.Add(topRight);
				}
			}
		}
		
	}

	void perlin_fbm(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float heightPowerBase, const float scalePowerBase, int iterations, const float height, double uvScale, bool genTriangles);
	void perlin(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float height, double uvScale, bool genTriangles);
	void morenoise(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float pointiness, const float scalingPowerBase, const int iterations, const float height, double uvScale, bool genTriangles);

	void cube(double3 offset, int resX, int resY, int resZ, double3 size, Mesh& out);
	void cube_without_uvs(double3 offset, double3 size, Mesh& out);

}

