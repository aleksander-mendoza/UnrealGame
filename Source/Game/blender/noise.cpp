#include "noise.h"
#include "Logging/StructuredLog.h"


namespace noise {

     int32 hash_bit_rotate(int32 x, int32 k)
    {
        return (x << k) | (x >> (32 - k));
    }

     void hash_bit_mix(int32& a, int32& b, int32& c)
    {
        a -= c;
        a ^= hash_bit_rotate(c, 4);
        c += b;
        b -= a;
        b ^= hash_bit_rotate(a, 6);
        a += c;
        c -= b;
        c ^= hash_bit_rotate(b, 8);
        b += a;
        a -= c;
        a ^= hash_bit_rotate(c, 16);
        c += b;
        b -= a;
        b ^= hash_bit_rotate(a, 19);
        a += c;
        c -= b;
        c ^= hash_bit_rotate(b, 4);
        b += a;
    }

     void hash_bit_final(int32& a, int32& b, int32& c)
    {
        c ^= b;
        c -= hash_bit_rotate(b, 14);
        a ^= c;
        a -= hash_bit_rotate(c, 11);
        b ^= a;
        b -= hash_bit_rotate(a, 25);
        c ^= b;
        c -= hash_bit_rotate(b, 16);
        a ^= c;
        a -= hash_bit_rotate(c, 4);
        b ^= a;
        b -= hash_bit_rotate(a, 14);
        c ^= b;
        c -= hash_bit_rotate(b, 24);
    }

    int32 hash(int32 kx)
    {
        int32 a, b, c;
        a = b = c = 0xdeadbeef + (1 << 2) + 13;

        a += kx;
        hash_bit_final(a, b, c);

        return c;
    }

    int32 hash(int32 kx, int32 ky)
    {
        int32 a, b, c;
        a = b = c = 0xdeadbeef + (2 << 2) + 13;

        b += ky;
        a += kx;
        hash_bit_final(a, b, c);

        return c;
    }

    int32 hash(int32 kx, int32 ky, int32 kz)
    {
        int32 a, b, c;
        a = b = c = 0xdeadbeef + (3 << 2) + 13;

        c += kz;
        b += ky;
        a += kx;
        hash_bit_final(a, b, c);

        return c;
    }

    int32 hash(int32 kx, int32 ky, int32 kz, int32 kw)
    {
        int32 a, b, c;
        a = b = c = 0xdeadbeef + (4 << 2) + 13;

        a += kx;
        b += ky;
        c += kz;
        hash_bit_mix(a, b, c);

        a += kw;
        hash_bit_final(a, b, c);

        return c;
    }

     int32 float_as_uint(float f)
    {
        union {
            int32 i;
            float f;
        } u;
        u.f = f;
        return u.i;
    }

    int32 hash_float(float kx)
    {
        return hash(float_as_uint(kx));
    }

    int32 hash_float(float2 k)
    {
        return hash(float_as_uint(k.X), float_as_uint(k.Y));
    }

    int32 hash_float(float3 k)
    {
        return hash(float_as_uint(k.X), float_as_uint(k.Y), float_as_uint(k.Z));
    }

    int32 hash_float(float4 k)
    {
        return hash(float_as_uint(k.X), float_as_uint(k.Y), float_as_uint(k.Z), float_as_uint(k.W));
    }


    /* Hashing a number of int32 into a float in the range [0, 1]. */

     float uint_to_float_01(int32 k)
    {
        return float(k) / float(0xFFFFFFFFu);
    }

    float hash_to_float(int32 kx)
    {
        return uint_to_float_01(hash(kx));
    }

    float hash_to_float(int32 kx, int32 ky)
    {
        return uint_to_float_01(hash(kx, ky));
    }

    float hash_to_float(int32 kx, int32 ky, int32 kz)
    {
        return uint_to_float_01(hash(kx, ky, kz));
    }

    float hash_to_float(int32 kx, int32 ky, int32 kz, int32 kw)
    {
        return uint_to_float_01(hash(kx, ky, kz, kw));
    }

    /* Hashing a number of floats into a float in the range [0, 1]. */

    float hash_float_to_float(float k)
    {
        return uint_to_float_01(hash_float(k));
    }

    float hash_float_to_float(float2 k)
    {
        return uint_to_float_01(hash_float(k));
    }

    float hash_float_to_float(float3 k)
    {
        return uint_to_float_01(hash_float(k));
    }

    float hash_float_to_float(float4 k)
    {
        return uint_to_float_01(hash_float(k));
    }

   

    float2 hash_float_to_float2(float3 k)
    {
        return float2(hash_float_to_float(float3(k.X, k.Y, k.Z)),
            hash_float_to_float(float3(k.Z, k.X, k.Y)));
    }

    float2 hash_float_to_float2(float4 k)
    {
        return float2(hash_float_to_float(float4(k.X, k.Y, k.Z, k.W)),
            hash_float_to_float(float4(k.Z, k.X, k.W, k.Y)));
    }

    float3 hash_float_to_float3(float k)
    {
        return float3(hash_float_to_float(k),
            hash_float_to_float(float2(k, 1.0)),
            hash_float_to_float(float2(k, 2.0)));
    }

    float3 hash_float_to_float3(float2 k)
    {
        return float3(hash_float_to_float(k),
            hash_float_to_float(float3(k.X, k.Y, 1.0)),
            hash_float_to_float(float3(k.X, k.Y, 2.0)));
    }

    float3 hash_float_to_float3(float3 k)
    {
        return float3(hash_float_to_float(k),
            hash_float_to_float(float4(k.X, k.Y, k.Z, 1.0)),
            hash_float_to_float(float4(k.X, k.Y, k.Z, 2.0)));
    }

    float3 hash_float_to_float3(float4 k)
    {
        return float3(hash_float_to_float(k),
            hash_float_to_float(float4(k.Z, k.X, k.W, k.Y)),
            hash_float_to_float(float4(k.W, k.Z, k.Y, k.X)));
    }

    float4 hash_float_to_float4(float4 k)
    {
        return float4(hash_float_to_float(k),
            hash_float_to_float(float4(k.W, k.X, k.Y, k.Z)),
            hash_float_to_float(float4(k.Z, k.W, k.X, k.Y)),
            hash_float_to_float(float4(k.Y, k.Z, k.W, k.X)));
    }


    float2 hash_float_to_float2(float2 k)
    {
        return float2(hash_float_to_float(k), hash_float_to_float(float3(k.X, k.Y, 1.0)));
    }


    float hash_to_float(int2 k)
    {
        return hash_to_float(k.X, k.Y);
    }

    float hash_to_float(int3 k)
    {
        return hash_to_float(k.X,k.Y,k.Z);
    }

    float hash_to_float(int4 k)
    {
        return hash_to_float(k.X,k.Y,k.Z,k.W);
    }


    float2 hash_to_float2(int32 k)
    {
        return float2(hash_to_float(k),
            hash_to_float(k, 1));
    }


    float2 hash_to_float2(int2 k)
    {
        return float2(hash_to_float(k), hash_to_float(k.X, k.Y, 1.0));
    }

    float2 hash_to_float2(int3 k)
    {
        return float2(hash_to_float(k.X, k.Y, k.Z),
            hash_to_float(k.Z, k.X, k.Y));
    }

    float2 hash_to_float2(int4 k)
    {
        return float2(hash_to_float(k.X, k.Y, k.Z, k.W),
            hash_to_float(k.Z, k.X, k.W, k.Y));
    }


    float3 hash_to_float3(int32 k)
    {
        return float3(hash_to_float(k),
            hash_to_float(k, 1),
            hash_to_float(k, 2));
    }

    float3 hash_to_float3(int2 k)
    {
        return float3(hash_to_float(k),
            hash_to_float(int3(k.X, k.Y, 1)),
            hash_to_float(int3(k.X, k.Y, 2)));
    }

    float3 hash_to_float3(int3 k)
    {
        return float3(hash_to_float(k),
            hash_to_float(k.X, k.Y, k.Z, 1),
            hash_to_float(k.X, k.Y, k.Z, 2));
    }

    float3 hash_to_float3(int4 k)
    {
        return float3(hash_to_float(k),
            hash_to_float(int4(k.Z, k.X, k.W, k.Y)),
            hash_to_float(int4(k.W, k.Z, k.Y, k.X)));
    }

    float4 hash_to_float4(int4 k)
    {
        return float4(hash_to_float(k),
            hash_to_float(int4(k.W, k.X, k.Y, k.Z)),
            hash_to_float(int4(k.Z, k.W, k.X, k.Y)),
            hash_to_float(int4(k.Y, k.Z, k.W, k.X)));
    }

    float4 hash_to_float4(int32 k) {
        return float3(hash_to_float(k),
            hash_to_float(k, 1),
            hash_to_float(k, 2));
    }


    /** \} */

    /* -------------------------------------------------------------------- */
    /** \name Perlin Noise
     *
     * Perlin, Ken. "Improving noise." Proceedings of the 29th annual conference on Computer graphics
     * and interactive techniques. 2002.
     *
     * This implementation is functionally identical to the implementations in EEVEE, OSL, and SVM. So
     * any changes should be applied in all relevant implementations.
     * \{ */

    

   

    inline float fade(float t)
    {
        return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
    }
    inline float fade_derivative(float t)
    {
        return t * t *  (t * (t * 5.0 * 6.0 - 4.0*15.0) + 3.0* 10.0);
    }

    float negate_if(float value, int32 condition)
    {
        return (condition != 0u) ? -value : value;
    }
    float negate_if_derivative(int32 condition)
    {
        return (condition != 0u) ? -1 : 1;
    }
    float noise_grad(int32 hash, float x)
    {
        int32 h = hash & 15u;
        float g = 1u + (h & 7u);
        return negate_if(g, h & 8u) * x;
    }

    float noise_grad(int32 hash, float x, float y)
    {
        int32 h = hash & 7u;
        float u = h < 4u ? x : y;
        float v = 2.0 * (h < 4u ? y : x);
        return negate_if(u, h & 1u) + negate_if(v, h & 2u);
    }

    float3 noise_grad_derivative(int32 hash, float x, float y)
    {
        const int32 h = hash & 7u;
        const uint8 maskx = h < 4u ? 1u : 2u;
        const uint8 masky = h < 4u ? 2u : 1u;
        float mulx = h < 4u ? 1.0 : 2.0;
        float muly = h < 4u ? 2.0 : 1.0;
        if ((h & maskx) != 0u) mulx = -mulx;
        if ((h & masky) != 0u) muly = -muly;
        const float r = mulx*x + muly*y;
        return float3(mulx, muly, r);
    }

    float noise_grad(int32 hash, float x, float y, float z)
    {
        int32 h = hash & 15u;
        float u = h < 8u ? x : y;
        float vt = ELEM(h, 12u, 14u) ? x : z;
        float v = h < 4u ? y : vt;
        return negate_if(u, h & 1u) + negate_if(v, h & 2u);
    }

    float noise_grad(int32 hash, float x, float y, float z, float w)
    {
        int32 h = hash & 31u;
        float u = h < 24u ? x : y;
        float v = h < 16u ? y : z;
        float s = h < 8u ? z : w;
        return negate_if(u, h & 1u) + negate_if(v, h & 2u) + negate_if(s, h & 4u);
    }

    

    float perlin_noise(float position)
    {
        int X;
        float fx = math::floor_fraction(position, X);
        float u = fade(fx);
        float v0 = noise_grad(hash(X), fx);
        float v1 = noise_grad(hash(X + 1), fx - 1.0);
        float r = mix(v0, v1, u);
        return r;
    }

    float perlin_noise(float2 position)
    {
        int X, Y;

        float fx = math::floor_fraction(position.X, X);
        float fy = math::floor_fraction(position.Y, Y);

        float u = fade(fx);
        float v = fade(fy);

        float r = mix(noise_grad(hash(X, Y), fx, fy),
            noise_grad(hash(X + 1, Y), fx - 1.0, fy),
            noise_grad(hash(X, Y + 1), fx, fy - 1.0),
            noise_grad(hash(X + 1, Y + 1), fx - 1.0, fy - 1.0),
            u,
            v);

        return r;
    }


    float3 perlin_noise_derivative(float2 position)
    {
        const int64 x = int64(position.X);
        const int64 y = int64(position.Y);
        const float fx = position.X - float(x);
        const float fy = position.Y - float(y);
        const float u = fade(fx);
        const float v = fade(fy);
        const float u_der = fade_derivative(fx);
        const float v_der = fade_derivative(fy);
        const int32 X = x;
        const int32 Y = y;
        
        //const float v0 = hash_to_float(X, Y);
        //const float v1 = hash_to_float(X + 1, Y);
        //const float v2 = hash_to_float(X, Y + 1);
        //const float v3 = hash_to_float(X + 1, Y + 1);
        //const float perlin_value = mix(v0,v1,v2,v3,u,v);
        //float2 gradient = mix_derivative(v0, v1, v2, v3, u, v);
        //return float3(gradient.X * u_der, gradient.Y * v_der, perlin_value);

        const float3 v0 = noise_grad_derivative(hash(X, Y), fx, fy);
        const float3 v1 = noise_grad_derivative(hash(X + 1, Y), fx - 1.0, fy);
        const float3 v2 = noise_grad_derivative(hash(X, Y + 1), fx, fy - 1.0);
        const float3 v3 = noise_grad_derivative(hash(X + 1, Y + 1), fx - 1.0, fy - 1.0);
        const float perlin_value = mix(v0.Z, v1.Z, v2.Z, v3.Z, u, v);
        
        // we make use of the property that
        // derivative of f(fx) wrt fx = derivative of f(fx) wrt x
        // from which follows
        // derivative of v0.Z wrt x (or y) = derivative of v0.X*x + v0.Y*y wrt x (or y)
        // we seek to compute
        // derivative of (1.0 - V(y)) * (v0(x,y) * (1 - U(x)) + v1(x,y) * U(x)) + V(y) * (v2(x,y) * (1 - U(x)) + v3(x,y) * U(x)) wrt x 
        // the notation v0(x,y) stands for v0.Z and says that this value depends on x and y. Similarly U(x) stands for u
        // feeding the above formula to wolfram alpha yeilds
        // (1 - V(y)) (-(U(x) - 1) v0^(1, 0)(x, y) + U'(x) (-v0(x, y)) + U(x) v1^(1, 0)(x, y) + U'(x) v1(x, y)) + V(y) (-(U(x) - 1) v2^(1, 0)(x, y) + U'(x) (-v2(x, y)) + U(x) v3^(1, 0)(x, y) + U'(x) v3(x, y))
        // where v0^(1, 0)(x, y) stands for v0.X and U'(x) = u_der thus giving us
        float der_x = (1. - v) * (-(u - 1.) * v0.X + u_der * -v0.Z + u * v1.X + u_der * v1.Z) + v * (-(u - 1.) * v2.X + u_der * (-v2.Z) + u * v3.X + u_der * v3.Z);
        // now for the y wolfram alpha yields
        // -(V'(y) (U(x) v1(x, y) - (U(x) - 1) v0(x, y))) + V'(y) (U(x) v3(x, y) - (U(x) - 1) v2(x, y)) + (1 - V(y)) (U(x) v1^(0, 1)(x, y) - (U(x) - 1) v0^(0, 1)(x, y)) + V(y) (U(x) v3^(0, 1)(x, y) - (U(x) - 1) v2^(0, 1)(x, y))
        float der_y = -(v_der* (u* v1.Z - (u - 1.)* v0.Z)) + v_der*(u* v3.Z - (u - 1.) * v2.Z) + (1. - v) * (u * v1.Y - (u - 1.) * v0.Y) + v* (u* v3.Y - (u - 1.)* v2.Y);


        return float3(der_x, der_y, perlin_value);
    }

    float perlin_noise(float3 position)
    {
        int X, Y, Z;

        float fx = math::floor_fraction(position.X, X);
        float fy = math::floor_fraction(position.Y, Y);
        float fz = math::floor_fraction(position.Z, Z);

        float u = fade(fx);
        float v = fade(fy);
        float w = fade(fz);

        float r = mix(noise_grad(hash(X, Y, Z), fx, fy, fz),
            noise_grad(hash(X + 1, Y, Z), fx - 1, fy, fz),
            noise_grad(hash(X, Y + 1, Z), fx, fy - 1, fz),
            noise_grad(hash(X + 1, Y + 1, Z), fx - 1, fy - 1, fz),
            noise_grad(hash(X, Y, Z + 1), fx, fy, fz - 1),
            noise_grad(hash(X + 1, Y, Z + 1), fx - 1, fy, fz - 1),
            noise_grad(hash(X, Y + 1, Z + 1), fx, fy - 1, fz - 1),
            noise_grad(hash(X + 1, Y + 1, Z + 1), fx - 1, fy - 1, fz - 1),
            u,
            v,
            w);

        return r;
    }

    float perlin_noise(float4 position)
    {
        int X, Y, Z, W;

        float fx = math::floor_fraction(position.X, X);
        float fy = math::floor_fraction(position.Y, Y);
        float fz = math::floor_fraction(position.Z, Z);
        float fw = math::floor_fraction(position.W, W);

        float u = fade(fx);
        float v = fade(fy);
        float t = fade(fz);
        float s = fade(fw);

        float r = mix(
            noise_grad(hash(X, Y, Z, W), fx, fy, fz, fw),
            noise_grad(hash(X + 1, Y, Z, W), fx - 1.0, fy, fz, fw),
            noise_grad(hash(X, Y + 1, Z, W), fx, fy - 1.0, fz, fw),
            noise_grad(hash(X + 1, Y + 1, Z, W), fx - 1.0, fy - 1.0, fz, fw),
            noise_grad(hash(X, Y, Z + 1, W), fx, fy, fz - 1.0, fw),
            noise_grad(hash(X + 1, Y, Z + 1, W), fx - 1.0, fy, fz - 1.0, fw),
            noise_grad(hash(X, Y + 1, Z + 1, W), fx, fy - 1.0, fz - 1.0, fw),
            noise_grad(hash(X + 1, Y + 1, Z + 1, W), fx - 1.0, fy - 1.0, fz - 1.0, fw),
            noise_grad(hash(X, Y, Z, W + 1), fx, fy, fz, fw - 1.0),
            noise_grad(hash(X + 1, Y, Z, W + 1), fx - 1.0, fy, fz, fw - 1.0),
            noise_grad(hash(X, Y + 1, Z, W + 1), fx, fy - 1.0, fz, fw - 1.0),
            noise_grad(hash(X + 1, Y + 1, Z, W + 1), fx - 1.0, fy - 1.0, fz, fw - 1.0),
            noise_grad(hash(X, Y, Z + 1, W + 1), fx, fy, fz - 1.0, fw - 1.0),
            noise_grad(hash(X + 1, Y, Z + 1, W + 1), fx - 1.0, fy, fz - 1.0, fw - 1.0),
            noise_grad(hash(X, Y + 1, Z + 1, W + 1), fx, fy - 1.0, fz - 1.0, fw - 1.0),
            noise_grad(hash(X + 1, Y + 1, Z + 1, W + 1), fx - 1.0, fy - 1.0, fz - 1.0, fw - 1.0),
            u,
            v,
            t,
            s);

        return r;
    }

    /* Signed versions of perlin noise in the range [-1, 1]. The scale values were computed
     * experimentally by the OSL developers to remap the noise output to the correct range. */

    float perlin_signed(float position)
    {
        float precision_correction = 0.5f * float(math::abs(position) >= 1000000.0f);
        /* Repeat Perlin noise texture every 100000.0 on each axis to prevent floating point
         * representation issues. */
        position = math::mod(position, 100000.0f) + precision_correction;

        return perlin_noise(position) * 0.2500f;
    }

    float perlin_signed(float2 position)
    {
        float2 precision_correction = 0.5f * float2(float(math::abs(position.X) >= 1000000.0f),
            float(math::abs(position.Y) >= 1000000.0f));
        /* Repeat Perlin noise texture every 100000.0f on each axis to prevent floating point
         * representation issues. This causes discontinuities every 100000.0f, however at such scales
         * this usually shouldn't be noticeable. */
        position = math::mod(position, 100000.0f) + precision_correction;

        return perlin_noise(position) * 0.6616f;
    }


    float perlin_signed(float3 position)
    {
        float3 precision_correction = 0.5f * float3(float(math::abs(position.X) >= 1000000.0f),
            float(fabsf(position.Y) >= 1000000.0f),
            float(math::abs(position.Z) >= 1000000.0f));
        /* Repeat Perlin noise texture every 100000.0f on each axis to prevent floating point
         * representation issues. This causes discontinuities every 100000.0f, however at such scales
         * this usually shouldn't be noticeable. */
        position = math::mod(position, 100000.0f) + precision_correction;

        return perlin_noise(position) * 0.9820f;
    }

    float perlin_signed(float4 position)
    {
        float4 precision_correction = 0.5f * float4(float(math::abs(position.X) >= 1000000.0f),
            float(math::abs(position.Y) >= 1000000.0f),
            float(math::abs(position.Z) >= 1000000.0f),
            float(math::abs(position.W) >= 1000000.0f));
        /* Repeat Perlin noise texture every 100000.0f on each axis to prevent floating point
         * representation issues. This causes discontinuities every 100000.0f, however at such scales
         * this usually shouldn't be noticeable. */
        position = math::mod(position, 100000.0f) + precision_correction;

        return perlin_noise(position) * 0.8344f;
    }

    /* Positive versions of perlin noise in the range [0, 1]. */

    float perlin(float position)
    {
        return perlin_signed(position) / 2.0f + 0.5f;
    }

    float perlin(float2 position)
    {
        return perlin_signed(position) / 2.0f + 0.5f;
    }

    float perlin(float3 position)
    {
        return perlin_signed(position) / 2.0f + 0.5f;
    }

    float perlin(float4 position)
    {
        return perlin_signed(position) / 2.0f + 0.5f;
    }


    /* Fractal perlin noise. */

    /* fBM = Fractal Brownian Motion */
    template<typename T>
    float perlin_fbm(
        T p, const float detail, const float roughness, const float lacunarity, const bool normalize)
    {
        float fscale = 1.0f;
        float amp = 1.0f;
        float maxamp = 0.0f;
        float sum = 0.0f;

        for (int i = 0; i <= int(detail); i++) {
            float t = perlin_signed(fscale * p);
            sum += t * amp;
            maxamp += amp;
            amp *= roughness;
            fscale *= lacunarity;
        }
        float rmd = detail - math::floor(detail);
        if (rmd != 0.0f) {
            float t = perlin_signed(fscale * p);
            float sum2 = sum + t * amp;
            return normalize ? mix(0.5f * sum / maxamp + 0.5f, 0.5f * sum2 / (maxamp + amp) + 0.5f, rmd) :
                mix(sum, sum2, rmd);
        }
        return normalize ? 0.5f * sum / maxamp + 0.5f : sum;
    }

    /* Explicit instantiation for Wave Texture. */
    template float perlin_fbm<float3>(float3 p,
        const float detail,
        const float roughness,
        const float lacunarity,
        const bool normalize);

    template float perlin_fbm<float2>(float2 p,
        const float detail,
        const float roughness,
        const float lacunarity,
        const bool normalize);

    template<typename T>
    float perlin_multi_fractal(T p, const float detail, const float roughness, const float lacunarity)
    {
        float value = 1.0f;
        float pwr = 1.0f;

        for (int i = 0; i <= int(detail); i++) {
            value *= (pwr * perlin_signed(p) + 1.0f);
            pwr *= roughness;
            p *= lacunarity;
        }

        const float rmd = detail - floorf(detail);
        if (rmd != 0.0f) {
            value *= (rmd * pwr * perlin_signed(p) + 1.0f); /* correct? */
        }

        return value;
    }

    template<typename T>
    float perlin_hetero_terrain(
        T p, const float detail, const float roughness, const float lacunarity, const float offset)
    {
        float pwr = roughness;

        /* First unscaled octave of function; later octaves are scaled. */
        float value = offset + perlin_signed(p);
        p *= lacunarity;

        for (int i = 1; i <= int(detail); i++) {
            float increment = (perlin_signed(p) + offset) * pwr * value;
            value += increment;
            pwr *= roughness;
            p *= lacunarity;
        }

        const float rmd = detail - floorf(detail);
        if (rmd != 0.0f) {
            float increment = (perlin_signed(p) + offset) * pwr * value;
            value += rmd * increment;
        }

        return value;
    }

    template<typename T>
    float perlin_hybrid_multi_fractal(T p,
        const float detail,
        const float roughness,
        const float lacunarity,
        const float offset,
        const float gain)
    {
        float pwr = 1.0f;
        float value = 0.0f;
        float weight = 1.0f;

        for (int i = 0; (weight > 0.001f) && (i <= int(detail)); i++) {
            if (weight > 1.0f) {
                weight = 1.0f;
            }

            float signal = (perlin_signed(p) + offset) * pwr;
            pwr *= roughness;
            value += weight * signal;
            weight *= gain * signal;
            p *= lacunarity;
        }

        const float rmd = detail - floorf(detail);
        if ((rmd != 0.0f) && (weight > 0.001f)) {
            if (weight > 1.0f) {
                weight = 1.0f;
            }
            float signal = (perlin_signed(p) + offset) * pwr;
            value += rmd * weight * signal;
        }

        return value;
    }

    template<typename T>
    float perlin_ridged_multi_fractal(T p,
        const float detail,
        const float roughness,
        const float lacunarity,
        const float offset,
        const float gain)
    {
        float pwr = roughness;

        float signal = offset - math::abs(perlin_signed(p));
        signal *= signal;
        float value = signal;
        float weight = 1.0f;

        for (int i = 1; i <= int(detail); i++) {
            p *= lacunarity;
            weight = math::clamp(signal * gain, 0.0f, 1.0f);
            signal = offset - math::abs(perlin_signed(p));
            signal *= signal;
            signal *= weight;
            value += signal * pwr;
            pwr *= roughness;
        }

        return value;
    }

    enum {
        NOISE_SHD_PERLIN_MULTIFRACTAL = 0,
        NOISE_SHD_PERLIN_FBM = 1,
        NOISE_SHD_PERLIN_HYBRID_MULTIFRACTAL = 2,
        NOISE_SHD_PERLIN_RIDGED_MULTIFRACTAL = 3,
        NOISE_SHD_PERLIN_HETERO_TERRAIN = 4,
    };

    template<typename T>
    float perlin_select(T p,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        int type,
        bool normalize)
    {
        switch (type) {
        case NOISE_SHD_PERLIN_MULTIFRACTAL: {
            return perlin_multi_fractal<T>(p, detail, roughness, lacunarity);
        }
        case NOISE_SHD_PERLIN_FBM: {
            return perlin_fbm<T>(p, detail, roughness, lacunarity, normalize);
        }
        case NOISE_SHD_PERLIN_HYBRID_MULTIFRACTAL: {
            return perlin_hybrid_multi_fractal<T>(p, detail, roughness, lacunarity, offset, gain);
        }
        case NOISE_SHD_PERLIN_RIDGED_MULTIFRACTAL: {
            return perlin_ridged_multi_fractal<T>(p, detail, roughness, lacunarity, offset, gain);
        }
        case NOISE_SHD_PERLIN_HETERO_TERRAIN: {
            return perlin_hetero_terrain<T>(p, detail, roughness, lacunarity, offset);
        }
        default: {
            return 0.0;
        }
        }
    }

    /* The following offset functions generate random offsets to be added to
     * positions to act as a seed since the noise functions don't have seed values.
     * The offset's components are in the range [100, 200], not too high to cause
     * bad precision and not too small to be noticeable. We use float seed because
     * OSL only supports float hashes and we need to maintain compatibility with it.
     */

    float random_float_offset(float seed)
    {
        return 100.0f + hash_float_to_float(seed) * 100.0f;
    }

    float2 random_float2_offset(float seed)
    {
        return float2(100.0f + hash_float_to_float(float2(seed, 0.0f)) * 100.0f,
            100.0f + hash_float_to_float(float2(seed, 1.0f)) * 100.0f);
    }

    float3 random_float3_offset(float seed)
    {
        return float3(100.0f + hash_float_to_float(float2(seed, 0.0f)) * 100.0f,
            100.0f + hash_float_to_float(float2(seed, 1.0f)) * 100.0f,
            100.0f + hash_float_to_float(float2(seed, 2.0f)) * 100.0f);
    }

    float4 random_float4_offset(float seed)
    {
        return float4(100.0f + hash_float_to_float(float2(seed, 0.0f)) * 100.0f,
            100.0f + hash_float_to_float(float2(seed, 1.0f)) * 100.0f,
            100.0f + hash_float_to_float(float2(seed, 2.0f)) * 100.0f,
            100.0f + hash_float_to_float(float2(seed, 3.0f)) * 100.0f);
    }

    /* Perlin noises to be added to the position to distort other noises. */

    float perlin_distortion(float position, float strength)
    {
        return perlin_signed(position + random_float_offset(0.0)) * strength;
    }

    float2 perlin_distortion(float2 position, float strength)
    {
        return float2(perlin_signed(position + random_float2_offset(0.0f)) * strength,
            perlin_signed(position + random_float2_offset(1.0f)) * strength);
    }

    float3 perlin_distortion(float3 position, float strength)
    {
        return float3(perlin_signed(position + random_float3_offset(0.0f)) * strength,
            perlin_signed(position + random_float3_offset(1.0f)) * strength,
            perlin_signed(position + random_float3_offset(2.0f)) * strength);
    }

    float4 perlin_distortion(float4 position, float strength)
    {
        return float4(perlin_signed(position + random_float4_offset(0.0f)) * strength,
            perlin_signed(position + random_float4_offset(1.0f)) * strength,
            perlin_signed(position + random_float4_offset(2.0f)) * strength,
            perlin_signed(position + random_float4_offset(3.0f)) * strength);
    }

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
        bool normalize)
    {
        position += perlin_distortion(position, distortion);
        return perlin_select<T>(position, detail, roughness, lacunarity, offset, gain, type, normalize);
    }

    template float perlin_fractal_distorted<float>(float position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);
    template float perlin_fractal_distorted<float2>(float2 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);
    template float perlin_fractal_distorted<float3>(float3 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);
    template float perlin_fractal_distorted<float4>(float4 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize);

    /* Distorted fractal perlin noise that outputs a float3. The arbitrary seeds are for
     * compatibility with shading functions. */

    float3 perlin_float3_fractal_distorted(float position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize)
    {
        position += perlin_distortion(position, distortion);
        return float3(
            perlin_select<float>(position, detail, roughness, lacunarity, offset, gain, type, normalize),
            perlin_select<float>(position + random_float_offset(1.0f),
                detail,
                roughness,
                lacunarity,
                offset,
                gain,
                type,
                normalize),
            perlin_select<float>(position + random_float_offset(2.0f),
                detail,
                roughness,
                lacunarity,
                offset,
                gain,
                type,
                normalize));
    }

    float3 perlin_float3_fractal_distorted(float2 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize)
    {
        position += perlin_distortion(position, distortion);
        return float3(perlin_select<float2>(
            position, detail, roughness, lacunarity, offset, gain, type, normalize),
            perlin_select<float2>(position + random_float2_offset(2.0f),
                detail,
                roughness,
                lacunarity,
                offset,
                gain,
                type,
                normalize),
            perlin_select<float2>(position + random_float2_offset(3.0f),
                detail,
                roughness,
                lacunarity,
                offset,
                gain,
                type,
                normalize));
    }

    float3 perlin_float3_fractal_distorted(float3 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize)
    {
        position += perlin_distortion(position, distortion);
        return float3(perlin_select<float3>(
            position, detail, roughness, lacunarity, offset, gain, type, normalize),
            perlin_select<float3>(position + random_float3_offset(3.0f),
                detail,
                roughness,
                lacunarity,
                offset,
                gain,
                type,
                normalize),
            perlin_select<float3>(position + random_float3_offset(4.0f),
                detail,
                roughness,
                lacunarity,
                offset,
                gain,
                type,
                normalize));
    }

    float3 perlin_float3_fractal_distorted(float4 position,
        float detail,
        float roughness,
        float lacunarity,
        float offset,
        float gain,
        float distortion,
        int type,
        bool normalize)
    {
        position += perlin_distortion(position, distortion);
        return float3(perlin_select<float4>(
            position, detail, roughness, lacunarity, offset, gain, type, normalize),
            perlin_select<float4>(position + random_float4_offset(4.0f),
                detail,
                roughness,
                lacunarity,
                offset,
                gain,
                type,
                normalize),
            perlin_select<float4>(position + random_float4_offset(5.0f),
                detail,
                roughness,
                lacunarity,
                offset,
                gain,
                type,
                normalize));
    }

    float3 random_vector(float3& min_value, float3& max_value, int id, int seed) {
        const float x = hash_to_float(seed, id, 0);
        const float y = hash_to_float(seed, id, 1);
        const float z = hash_to_float(seed, id, 2);
        return float3(x, y, z) * (max_value - min_value) + min_value;
    }
    float random_float(float min_value, float max_value, int id, int seed)  {
        const float value = hash_to_float(seed, id);
        return value * (max_value - min_value) + min_value;
    }
    int random_int(int min_value, int max_value, int id, int seed)  {
        const float value = hash_to_float(id, seed);
        /* Add one to the maximum and use floor to produce an even
         * distribution for the first and last values (See #93591). */
        return floor(value * (max_value + 1 - min_value) + min_value);
    }
    bool random_bool(float probability, int id, int seed)  {
        return hash_to_float(id, seed) <= probability;
    }

}