
#pragma once

#include "CoreMinimal.h"
#include "utildefines.h"

namespace noise {
    int32 float_as_uint(float f);

	int32 hash(int32 kx);
	int32 hash(int32 kx, int32 ky);
	int32 hash(int32 kx, int32 ky, int32 kz);
	int32 hash(int32 kx, int32 ky, int32 kz, int32 kw);

	/* Hash floats to `int32`. */

	int32 hash_float(float kx);
	int32 hash_float(float2 k);
	int32 hash_float(float3 k);
	int32 hash_float(float3 k);
    int32 hash_float(float4 k);

	/* Hash integers to `float` between 0 and 1. */

	float hash_to_float(int32 kx);
	float hash_to_float(int32 kx, int32 ky);
	float hash_to_float(int32 kx, int32 ky, int32 kz);
	float hash_to_float(int32 kx, int32 ky, int32 kz, int32 kw);

	/* Hash floats to `float` between 0 and 1. */

	float hash_float_to_float(float k);
	float hash_float_to_float(float2 k);
	float hash_float_to_float(float3 k);
	float hash_float_to_float(float4 k);

    float2 hash_float_to_float2(float2 k);
	float2 hash_float_to_float2(float3 k);
	float2 hash_float_to_float2(float4 k);

	float3 hash_float_to_float3(float k);
	float3 hash_float_to_float3(float2 k);
	float3 hash_float_to_float3(float3 k);
	float3 hash_float_to_float3(float4 k);
	float4 hash_float_to_float4(float4 k);


    float  hash_to_float (int2 k);
    float  hash_to_float (int3 k);
    float  hash_to_float (int4 k);
    float2 hash_to_float2(int32 k);
    float2 hash_to_float2(int2 k);
    float2 hash_to_float2(int3 k);
    float2 hash_to_float2(int4 k);
    float3 hash_to_float3(int32 k);
    float3 hash_to_float3(int2 k);
    float3 hash_to_float3(int3 k);
    float3 hash_to_float3(int4 k);
    float4 hash_to_float4(int4 k);
    float4 hash_to_float4(int32 k);

    /* Linear Interpolation. */
    template<typename T> T inline mix(T v0, T v1, float x)
    {
        return (1 - x) * v0 + x * v1;
    }
    template<typename T> T inline mix_derivative(T v0, T v1, float x)
    {
        return v1 - v0;
    }
    /* Bilinear Interpolation:
    *
    * v2          v3
    *  @ + + + + @       y
    *  +         +       ^
    *  +         +       |
    *  +         +       |
    *  @ + + + + @       @------> x
    * v0          v1
    */
    inline float mix(float v0, float v1, float v2, float v3, float x, float y)
    {
        float x1 = 1.0 - x;
        return (1.0 - y) * (v0 * x1 + v1 * x) + y * (v2 * x1 + v3 * x);
    }
    inline float2 mix_derivative(float v0, float v1, float v2, float v3, float x, float y)
    {
        float v = v0 - v1 - v2 + v3;
        return float2(
            -v0 + v1 + v*y,
            v2  - v0 + v*x
        );
    }

    /* Trilinear Interpolation:
     *
     *   v6               v7
     *     @ + + + + + + @
     *     +\            +\
     *     + \           + \
     *     +  \          +  \
     *     +   \ v4      +   \ v5
     *     +    @ + + + +++ + @          z
     *     +    +        +    +      y   ^
     *  v2 @ + +++ + + + @ v3 +       \  |
     *      \   +         \   +        \ |
     *       \  +          \  +         \|
     *        \ +           \ +          +---------> x
     *         \+            \+
     *          @ + + + + + + @
     *        v0               v1
     */
    inline float mix(float v0,
        float v1,
        float v2,
        float v3,
        float v4,
        float v5,
        float v6,
        float v7,
        float x,
        float y,
        float z)
    {
        float x1 = 1.0 - x;
        float y1 = 1.0 - y;
        float z1 = 1.0 - z;
        return z1 * (y1 * (v0 * x1 + v1 * x) + y * (v2 * x1 + v3 * x)) +
            z * (y1 * (v4 * x1 + v5 * x) + y * (v6 * x1 + v7 * x));
        
    }

    inline float3 mix_derivative(float v0,
        float v1,
        float v2,
        float v3,
        float v4,
        float v5,
        float v6,
        float v7,
        float x,
        float y,
        float z)
    {
        float x1 = 1.0 - x;
        float y1 = 1.0 - y;
        float z1 = 1.0 - z;
        float v01x = v0 * x1 + v1 * x;
        float v23x = v2 * x1 + v3 * x;
        float v45x = v4 * x1 + v5 * x;
        float v67x = v6 * x1 + v7 * x;
        // you can feed this to worfram alpha
        // ( 1.0 - z) * ((1.0 - y) * (v0 * (1.0 - x) + v1 * x) + y * (v2 * (1.0 - x) + v3 * x)) + z* ((1.0 - y) * (v4 * (1.0 - x) + v5 * x) + y * (v6 * (1.0 - x) + v7 * x))
        return float3(
            z1 * (y1 * (-v0 + v1) + y * (-v2 + v3 )) +
            z * (y1 * (-v4 + v5) + y * (-v6 + v7)),
            z1 * ( -v01x + v23x) +
            z * (-v45x + v67x),
            -(y1 * v01x + y * v23x) +
            (y1 * v45x + y * v67x)
        );
    }

    /* Quadrilinear Interpolation. */
    inline float mix(float v0,
        float v1,
        float v2,
        float v3,
        float v4,
        float v5,
        float v6,
        float v7,
        float v8,
        float v9,
        float v10,
        float v11,
        float v12,
        float v13,
        float v14,
        float v15,
        float x,
        float y,
        float z,
        float w)
    {
        return mix(mix(v0, v1, v2, v3, v4, v5, v6, v7, x, y, z),
            mix(v8, v9, v10, v11, v12, v13, v14, v15, x, y, z),
            w);
    }

    float3 perlin_noise_derivative(float2 position);

    float perlin_noise(float position);
    float perlin_noise(float2 position);
    float perlin_noise(float3 position);
    float perlin_noise(float4 position);

	/* Perlin noise in the range [-1, 1]. */

	float perlin_signed(float position);
	float perlin_signed(float2 position);
	float perlin_signed(float3 position);
	float perlin_signed(float4 position);

	/* Perlin noise in the range [0, 1]. */

	float perlin(float position);
	float perlin(float2 position);
	float perlin(float3 position);
	float perlin(float4 position);


    /* Perlin fractal Brownian motion. */

    template<typename T>
    float perlin_fbm(T p, float detail, float roughness, float lacunarity, bool normalize);

    /* Distorted fractal perlin noise. */

    template<typename T>
    float perlin_fractal_distorted(T position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);

    /* Distorted fractal perlin noise that outputs a float3. */

    float3 perlin_float3_fractal_distorted(float position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);
    float3 perlin_float3_fractal_distorted(float2 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);
    float3 perlin_float3_fractal_distorted(float3 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);
    float3 perlin_float3_fractal_distorted(float4 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);

    float3 random_vector(float3& min_value, float3& max_value, int id, int seed);
    float random_float(float min_value, float max_value, int id, int seed);
    int random_int(int min_value, int max_value, int id, int seed);
    bool random_bool(float probability, int id, int seed);


}

