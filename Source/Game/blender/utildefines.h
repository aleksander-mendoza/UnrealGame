// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/* --- internal helpers --- */
#define _VA_NARGS_GLUE(x, y) x y
#define _VA_NARGS_RETURN_COUNT(\
  _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, \
  _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, \
  _33_, _34_, _35_, _36_, _37_, _38_, _39_, _40_, _41_, _42_, _43_, _44_, _45_, _46_, _47_, _48_, \
  _49_, _50_, _51_, _52_, _53_, _54_, _55_, _56_, _57_, _58_, _59_, _60_, _61_, _62_, _63_, _64_, \
  count, ...) count
#define _VA_NARGS_EXPAND(args) _VA_NARGS_RETURN_COUNT args
#define _VA_NARGS_OVERLOAD_MACRO2(name, count) name##count
#define _VA_NARGS_OVERLOAD_MACRO1(name, count) _VA_NARGS_OVERLOAD_MACRO2(name, count)
#define _VA_NARGS_OVERLOAD_MACRO(name,  count) _VA_NARGS_OVERLOAD_MACRO1(name, count)
/* --- expose for re-use --- */
/* 64 args max */
#define VA_NARGS_COUNT(...) _VA_NARGS_EXPAND((__VA_ARGS__, \
  64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
  48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
  32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
  16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2, 1, 0))
#define VA_NARGS_CALL_OVERLOAD(name, ...) \
  _VA_NARGS_GLUE(_VA_NARGS_OVERLOAD_MACRO(name, VA_NARGS_COUNT(__VA_ARGS__)), (__VA_ARGS__))


/* ELEM#(v, ...): is the first arg equal any others? */
/* internal helpers. */
#define _VA_ELEM2(v, a) ((v) == (a))
#define _VA_ELEM3(v, a, b) \
  (_VA_ELEM2(v, a) || _VA_ELEM2(v, b))
#define _VA_ELEM4(v, a, b, c) \
  (_VA_ELEM3(v, a, b) || _VA_ELEM2(v, c))
#define _VA_ELEM5(v, a, b, c, d) \
  (_VA_ELEM4(v, a, b, c) || _VA_ELEM2(v, d))
#define _VA_ELEM6(v, a, b, c, d, e) \
  (_VA_ELEM5(v, a, b, c, d) || _VA_ELEM2(v, e))
#define _VA_ELEM7(v, a, b, c, d, e, f) \
  (_VA_ELEM6(v, a, b, c, d, e) || _VA_ELEM2(v, f))
#define _VA_ELEM8(v, a, b, c, d, e, f, g) \
  (_VA_ELEM7(v, a, b, c, d, e, f) || _VA_ELEM2(v, g))
#define _VA_ELEM9(v, a, b, c, d, e, f, g, h) \
  (_VA_ELEM8(v, a, b, c, d, e, f, g) || _VA_ELEM2(v, h))
#define _VA_ELEM10(v, a, b, c, d, e, f, g, h, i) \
  (_VA_ELEM9(v, a, b, c, d, e, f, g, h) || _VA_ELEM2(v, i))
#define _VA_ELEM11(v, a, b, c, d, e, f, g, h, i, j) \
  (_VA_ELEM10(v, a, b, c, d, e, f, g, h, i) || _VA_ELEM2(v, j))
#define _VA_ELEM12(v, a, b, c, d, e, f, g, h, i, j, k) \
  (_VA_ELEM11(v, a, b, c, d, e, f, g, h, i, j) || _VA_ELEM2(v, k))
#define _VA_ELEM13(v, a, b, c, d, e, f, g, h, i, j, k, l) \
  (_VA_ELEM12(v, a, b, c, d, e, f, g, h, i, j, k) || _VA_ELEM2(v, l))
#define _VA_ELEM14(v, a, b, c, d, e, f, g, h, i, j, k, l, m) \
  (_VA_ELEM13(v, a, b, c, d, e, f, g, h, i, j, k, l) || _VA_ELEM2(v, m))
#define _VA_ELEM15(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  (_VA_ELEM14(v, a, b, c, d, e, f, g, h, i, j, k, l, m) || _VA_ELEM2(v, n))
#define _VA_ELEM16(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) \
  (_VA_ELEM15(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n) || _VA_ELEM2(v, o))
#define _VA_ELEM17(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
  (_VA_ELEM16(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) || _VA_ELEM2(v, p))
/* clang-format on */

/* reusable ELEM macro */
#define ELEM(...) VA_NARGS_CALL_OVERLOAD(_VA_ELEM, __VA_ARGS__)

typedef FVector4f float4;
typedef FVector3f float3;
typedef FVector2f float2;
typedef FVector4d double4;
typedef FVector3d double3;
typedef FVector2d double2;
typedef FIntVector4 int4;
typedef FIntVector3 int3;
typedef FIntVector2 int2;

namespace math {
    inline float add(const float a, float b)
    {
        return a + b;
    }
    inline float2 add(const float2& a, const float2& b)
    {
        return float2(add(a.X, b.X), add(a.Y, b.Y));
    }
    inline float3 add(const float3& a, const float3& b)
    {
        return float3(add(a.X, b.X), add(a.Y, b.Y), add(a.Z, b.Z));
    }
    inline float4 add(const float4& a, const float4& b)
    {
        return float4(add(a.X, b.X), add(a.Y, b.Y), add(a.Z, b.Z), add(a.W, b.W));
    }
    inline float2 add(const float2& a, const float b)
    {
        return float2(add(a.X, b), add(a.Y, b));
    }
    inline float3 add(const float3& a, const float b)
    {
        return float3(add(a.X, b), add(a.Y, b), add(a.Z, b));
    }
    inline float4 add(const float4& a, const float b)
    {
        return float4(add(a.X, b), add(a.Y, b), add(a.Z, b), add(a.W, b));
    }

    inline float sub(const float a, float b)
    {
        return a - b;
    }
    inline float2 sub(const float2& a, const float2& b)
    {
        return float2(sub(a.X, b.X), sub(a.Y, b.Y));
    }
    inline float3 sub(const float3& a, const float3& b)
    {
        return float3(sub(a.X, b.X), sub(a.Y, b.Y), sub(a.Z, b.Z));
    }
    inline float4 sub(const float4& a, const float4& b)
    {
        return float4(sub(a.X, b.X), sub(a.Y, b.Y), sub(a.Z, b.Z), sub(a.W, b.W));
    }
    inline float2 sub(const float2& a, const float b)
    {
        return float2(sub(a.X, b), sub(a.Y, b));
    }
    inline float3 sub(const float3& a, const float b)
    {
        return float3(sub(a.X, b), sub(a.Y, b), sub(a.Z, b));
    }
    inline float4 sub(const float4& a, const float b)
    {
        return float4(sub(a.X, b), sub(a.Y, b), sub(a.Z, b), sub(a.W, b));
    }

    inline float mul(const float a, float b)
    {
        return a * b;
    }
    inline float2 mul(const float2& a, const float2& b)
    {
        return float2(mul(a.X, b.X), mul(a.Y, b.Y));
    }
    inline float3 mul(const float3& a, const float3& b)
    {
        return float3(mul(a.X, b.X), mul(a.Y, b.Y), mul(a.Z, b.Z));
    }
    inline float4 mul(const float4& a, const float4& b)
    {
        return float4(mul(a.X, b.X), mul(a.Y, b.Y), mul(a.Z, b.Z), mul(a.W, b.W));
    }
    inline float2 mul(const float2& a, const float b)
    {
        return float2(mul(a.X, b), mul(a.Y, b));
    }
    inline float3 mul(const float3& a, const float b)
    {
        return float3(mul(a.X, b), mul(a.Y, b), mul(a.Z, b));
    }
    inline float4 mul(const float4& a, const float b)
    {
        return float4(mul(a.X, b), mul(a.Y, b), mul(a.Z, b), mul(a.W, b));
    }

    inline float div(const float a, float b)
    {
        return a / b;
    }
    inline float2 div(const float2& a, const float2& b)
    {
        return float2(div(a.X, b.X), div(a.Y, b.Y));
    }
    inline float3 div(const float3& a, const float3& b)
    {
        return float3(div(a.X, b.X), div(a.Y, b.Y), div(a.Z, b.Z));
    }
    inline float4 div(const float4& a, const float4& b)
    {
        return float4(div(a.X, b.X), div(a.Y, b.Y), div(a.Z, b.Z), div(a.W, b.W));
    }
    inline float2 div(const float2& a, const float b)
    {
        return float2(div(a.X, b), div(a.Y, b));
    }
    inline float3 div(const float3& a, const float b)
    {
        return float3(div(a.X, b), div(a.Y, b), div(a.Z, b));
    }
    inline float4 div(const float4& a, const float b)
    {
        return float4(div(a.X, b), div(a.Y, b), div(a.Z, b), div(a.W, b));
    }


    inline float mod(const float a, float b)
    {
        return modff(a, &b);
    }
    inline float2 mod(const float2& a, const float2& b)
    {
        return float2(mod(a.X,b.X), mod(a.Y, b.Y));
    }
    inline float3 mod(const float3& a, const float3& b)
    {
        return float3(mod(a.X, b.X), mod(a.Y, b.Y), mod(a.Z, b.Z));
    }
    inline float4 mod(const float4& a, const float4& b)
    {
        return float4(mod(a.X, b.X), mod(a.Y, b.Y), mod(a.Z, b.Z), mod(a.W, b.W));
    }
    inline float2 mod(const float2& a, const float b)
    {
        return float2(mod(a.X, b), mod(a.Y, b));
    }
    inline float3 mod(const float3& a, const float b)
    {
        return float3(mod(a.X, b), mod(a.Y, b), mod(a.Z, b));
    }
    inline float4 mod(const float4& a, const float b)
    {
        return float4(mod(a.X, b), mod(a.Y, b), mod(a.Z, b), mod(a.W, b));
    }

    inline int abs(const int a)
    {
        return a>0?a:-a;
    }
    inline int2 abs(const int2& a)
    {
        return int2(abs(a.X), abs(a.Y));
    }
    inline int3 abs(const int3& a)
    {
        return int3(abs(a.X), abs(a.Y), abs(a.Z));
    }
    inline int4 abs(const int4& a)
    {
        return int4(abs(a.X), abs(a.Y), abs(a.Z), abs(a.W));
    }


    inline double abs(const double a)
    {
        return fabs(a);
    }
    inline float abs(const float a)
    {
        return fabsf(a);
    }
    inline float2 abs(const float2& a)
    {
        return float2(abs(a.X), abs(a.Y));
    }
    inline float3 abs(const float3& a)
    {
        return float3(abs(a.X), abs(a.Y), abs(a.Z));
    }
    inline float4 abs(const float4& a)
    {
        return float4(abs(a.X), abs(a.Y), abs(a.Z), abs(a.W));
    }

    inline int sum(const int a)
    {
        return a;
    }
    inline int sum(const int2& a)
    {
        return a.X + a.Y;
    }
    inline int sum(const int3& a)
    {
        return a.X + a.Y + a.Z;
    }
    inline int sum(const int4& a)
    {
        return a.X + a.Y + a.Z + a.W;
    }
    inline float sum(const float a)
    {
        return a;
    }
    inline float sum(const float2& a)
    {
        return a.X+a.Y;
    }
    inline float sum(const float3& a)
    {
        return a.X+a.Y+a.Z;
    }
    inline float sum(const float4& a)
    {
        return a.X + a.Y + a.Z + a.W;
    }

    inline float sqrt(const float a)
    {
        return sqrtf(a);
    }
    inline float2 sqrt(const float2& a)
    {
        return float2(sqrt(a.X), sqrt(a.Y));
    }
    inline float3 sqrt(const float3& a)
    {
        return float3(sqrt(a.X), sqrt(a.Y), sqrt(a.Z));
    }
    inline float4 sqrt(const float4& a)
    {
        return float4(sqrt(a.X), sqrt(a.Y), sqrt(a.Z), sqrt(a.W));
    }

    inline float floor(const float a)
    {
        return floorf(a);
    }
    inline float2 floor(const float2& a)
    {
        return float2(floor(a.X), floor(a.Y));
    }
    inline float3 floor(const float3& a)
    {
        return float3(floor(a.X), floor(a.Y), floor(a.Z));
    }
    inline float4 floor(const float4& a)
    {
        return float4(floor(a.X), floor(a.Y), floor(a.Z), floor(a.W));
    }

    inline int floori(const float a)
    {
        return floorf(a);
    }
    inline int2 floori(const float2& a)
    {
        return int2(floori(a.X), floori(a.Y));
    }
    inline int3 floori(const float3& a)
    {
        return int3(floori(a.X), floori(a.Y), floori(a.Z));
    }
    inline int4 floori(const float4& a)
    {
        return int4(floori(a.X), floori(a.Y), floori(a.Z), floori(a.W));
    }

    inline int ceili(const float a)
    {
        return ceilf(a);
    }
    inline int2 ceili(const float2& a)
    {
        return int2(ceili(a.X), ceili(a.Y));
    }
    inline int3 ceili(const float3& a)
    {
        return int3(ceili(a.X), ceili(a.Y), ceili(a.Z));
    }
    inline int4 ceili(const float4& a)
    {
        return int4(ceili(a.X), ceili(a.Y), ceili(a.Z), ceili(a.W));
    }

    inline float min(const float a, const float b)
    {
        return a < b ? a : b;
    }
    inline float max(const float a, const float b)
    {
        return a > b ? a : b;
    }
    inline float clamp(const float a, const float min_val, const float max_val)
    {
        return max(min(a, max_val), min_val);
    }
    inline float2 clamp(const float2& a, const float min_val, const float max_val)
    {
        return float2(clamp(a.X,min_val, max_val), clamp(a.Y, min_val, max_val));
    }
    inline float3 clamp(const float3& a, const float min_val, const float max_val)
    {
        return float3(clamp(a.X, min_val, max_val), clamp(a.Y, min_val, max_val), clamp(a.Z, min_val, max_val));
    }
    inline float4 clamp(const float4& a, const float min_val, const float max_val)
    {
        return float4(clamp(a.X, min_val, max_val), clamp(a.Y, min_val, max_val), clamp(a.Z, min_val, max_val), clamp(a.W, min_val, max_val));
    }

    inline float step(const float edge, const float value)
    {
        return value < edge ? 0 : 1;
    }

    template <typename T> T sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    inline float floor_fraction(float x, int& i)
    {
        float x_floor = floor(x);
        i = int(x_floor);
        return x - x_floor;
    }
    inline float2 floor_fraction(float2 x, int2& i)
    {
        return float2(floor_fraction(x.X, i.X), floor_fraction(x.Y, i.Y));
    }
    inline float3 floor_fraction(float3 x, int3& i)
    {
        return float3(floor_fraction(x.X, i.X), floor_fraction(x.Y, i.Y), floor_fraction(x.Z, i.Z));
    }
    inline float fraction(float x)
    {
        return x - floor(x);
    }
    inline float2 fraction(float2 x)
    {
        return float2(fraction(x.X), fraction(x.Y));
    }
    inline float3 fraction(float3 x)
    {
        return float3(fraction(x.X), fraction(x.Y), fraction(x.Z));
    }
    

    inline int dot(int v1, int v2)
    {
        return v1 * v2;
    }
    inline int dot(int2 v1, int2 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y;
    }
    inline int dot(int3 v1, int3 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y
            + v1.Z * v2.Z;
    }
    inline int dot(int4 v1, int4 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y
            + v1.Z * v2.Z
            + v1.W * v2.W;
    }
    inline float dot(float v1, float v2)
    {
        return v1 * v2;
    }
    inline float dot(float2 v1, float2 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y;
    }
    inline float dot(float3 v1, float3 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y
            + v1.Z * v2.Z;
    }
    inline float dot(float4 v1, float4 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y
            + v1.Z * v2.Z
            + v1.W * v2.W;
    }
    inline double dot(double v1, double v2)
    {
        return v1 * v2;
    }
    inline double dot(double2 v1, double2 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y;
    }
    inline double dot(double3 v1, double3 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y
            + v1.Z * v2.Z;
    }
    inline double dot(double4 v1, double4 v2)
    {
        return v1.X * v2.X
            + v1.Y * v2.Y
            + v1.Z * v2.Z
            + v1.W * v2.W;
    }

    inline int lengthSquare(int v1)
    {
        return dot(v1, v1);
    }
    inline int lengthSquare(int2 v1)
    {
        return dot(v1, v1);
    }
    inline int lengthSquare(int3 v1)
    {
        return dot(v1, v1);
    }
    inline int lengthSquare(int4 v1)
    {
        return dot(v1, v1);
    }
    inline float length(float v1)
    {
        return abs(v1);
    }
    inline float length(float2 v1)
    {
        return sqrt(dot(v1,v1));
    }
    inline float length(float3 v1)
    {
        return sqrt(dot(v1, v1));
    }
    inline float length(float4 v1)
    {
        return sqrt(dot(v1, v1));
    }
    inline double length(double v1)
    {
        return abs(v1);
    }
    inline double length(double2 v1)
    {
        return sqrt(dot(v1, v1));
    }
    inline double length(double3 v1)
    {
        return sqrt(dot(v1, v1));
    }
    inline double length(double4 v1)
    {
        return sqrt(dot(v1, v1));
    }

    inline int l1Dist(int v1, int v2)
    {
        return sum(abs(v1-v2));
    }
    inline int l1Dist(int2 v1, int2 v2)
    {
        return sum(abs(v1-v2));
    }
    inline int l1Dist(int3 v1, int3 v2)
    {
        return sum(abs(v1-v2));
    }
    inline int l1Dist(int4 v1, int4 v2)
    {
        return sum(abs(v1-v2));
    }
    inline int l1Dist(float v1, float v2)
    {
        return sum(abs(v1-v2));
    }
    inline float l1Dist(float2 v1, float2 v2)
    {
        return sum(abs(v1-v2));
    }
    inline float l1Dist(float3 v1, float3 v2)
    {
        return sum(abs(v1-v2));
    }
    inline float l1Dist(float4 v1, float4 v2)
    {
        return sum(abs(v1-v2));
    }
    inline float dist(float v1, float v2)
    {
        return length(sub(v1,v2));
    }
    inline float dist(float2 v1, float2 v2)
    {
        return length(v1-v2);
    }
    inline float dist(float3 v1, float3 v2)
    {
        return length(v1-v2);
    }
    inline float dist(float4 v1, float4 v2)
    {
        return length(v1-v2);
    }

    inline int distSquare(int v1, int v2)
    {
        return lengthSquare(v1-v2);
    }
    inline int distSquare(int2 v1, int2 v2)
    {
        return lengthSquare(v1 - v2);
    }
    inline int distSquare(int3 v1, int3 v2)
    {
        return lengthSquare(v1 - v2);
    }
    inline int distSquare(int4 v1, int4 v2)
    {
        return lengthSquare(v1 - v2);
    }
    inline float3 cross(float3 v1, float3 v2)
    {
        return FVector3f::CrossProduct(v1,v2);
    }
    inline float3 normal(float2 gradient)
    {
        return cross(float3(1,0, gradient.X), float3(0, 1, gradient.Y));
    }
    inline double3 cross(double3 v1, double3 v2)
    {
        return FVector3d::CrossProduct(v1, v2);
    }
    inline double3 normal(double2 gradient)
    {
        return cross(double3(1, 0, gradient.X), double3(0, 1, gradient.Y));
    }
    inline float3 tangent(float2 gradient)
    {
        return float3(1, 1, gradient.X+ gradient.Y);
    }
    inline float normalize(float v) {
        return 1;
    }
    inline float2 normalize(float2 v) {
        return v/ length(v);
    }
    inline float3 normalize(float3 v) {
        return v/ length(v);
    }
    inline float4 normalize(float4 v) {
        return v/length(v);
    }
    inline double normalize(double v) {
        return 1;
    }
    inline double2 normalize(double2 v) {
        return v / length(v);
    }
    inline double3 normalize(double3 v) {
        return v / length(v);
    }
    inline double4 normalize(double4 v) {
        return v / length(v);
    }
    inline float3 cross_tri(float3 v1, float3 v2, float3 v3)
    {
        return cross(v1-v2, sub(v3, v2));
    }
    inline float area_tri(float3 v1, float3 v2, float3 v3)
    {
        return length(cross_tri(v1, v2, v3))*0.5f;
    }
    inline float3 interp_v3_v3v3v3( const float3 v1, const float3 v2, const float3 v3, const float3 w)
    {
        return v1*w.X+ v2 * w.X+ v3 * w.Z;
    }
}