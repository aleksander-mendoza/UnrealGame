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
	};

	template<typename F>
	void grid(double3 offset, int resX, int resY, float2 size, Mesh & out, F heightFunction) {
		const float spacingX = size.X / (float)(resX - 1);
		const float spacingY = size.Y / (float)(resY - 1);
		TArray<FVector> vertices;
		out.vertices.Reserve(resY * resX);
		TArray<FVector2D> uvs;
		TArray<int32> triangles;
		out.triangles.Reserve((resY - 1) * (resX - 1) * 2);
		TArray<FVector> normals;
		out.normals.Reserve(resY * resX);
		TArray<FProcMeshTangent> tangents;
		out.normals.Reserve(resY * resX);

		for (int32 vy = 0; vy < resY; vy++) {
			for (int32 vx = 0; vx < resX; vx++) {
				FVector vertex = offset;
				vertex.X += float(vx) * spacingX;
				vertex.Y += float(vy) * spacingY;
				float3 derivative_and_height = heightFunction(vertex);

				vertex.Z += derivative_and_height.Z;
				out.vertices.Add(vertex);
				FVector2D uv(vertex);
				out.uvs.Add(uv);
				float3 normal = math::normalize(math::normal(float2(derivative_and_height)));
				out.normals.Add(FVector(normal));
				float3 tangent = math::normalize(math::tangent(float2(derivative_and_height)));
				out.tangents.Add(FProcMeshTangent(FVector(tangent), false));
			}
		}


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

	void perlin(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float height);
	void morenoise(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float pointiness, const float scalingPowerBase, const float iterations, const float height);

	void cube(double3 offset, int resX, int resY, int resZ, float3 size, Mesh& out);
}