#include "proc_assets.h"
#include "noise.h"

namespace proc_assets {
	void perlin(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float height) {
		grid(offset, resX, resY, size, out, [scale, height](FVector& vertex) {return noise::perlin_noise_derivative(float2(vertex.X / scale, vertex.Y / scale)) * height; });
	}
	void morenoise(double3 offset, int resX, int resY, float2 size, Mesh& out, const float scale, const float pointiness, const float scalingPowerBase, const float iterations, const float height) {
		grid(offset, resX, resY, size, out, [scale, height, pointiness, scalingPowerBase, iterations](FVector& vertex) {return noise::morenoise(float2(vertex.X / scale, vertex.Y / scale), pointiness, scalingPowerBase, iterations) * height; });
	}
	void cube(double3 offset, int resX, int resY, int resZ, float3 size, Mesh& out) {

	}
}