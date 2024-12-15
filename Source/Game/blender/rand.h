#pragma once
/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

 /** \file
  * \ingroup bli
  */

#include<assert.h>
#include "CoreMinimal.h"
#include "utildefines.h"

namespace blender {

    class RandomNumberGenerator {
    private:
        uint64 x_;

    public:
        RandomNumberGenerator(uint32 seed = 0)
        {
            this->seed(seed);
        }

        /**
         * Creates a random number generator with a somewhat random seed. This can be used when
         * determinism is not necessary or not desired.
         */
        static RandomNumberGenerator from_random_seed();

        /**
         * Set the seed for future random numbers.
         */
        void seed(uint32 seed)
        {
            constexpr uint64 lowseed = 0x330E;
            x_ = (uint64(seed) << 16) | lowseed;
        }

        /**
         * Set a randomized hash of the value as seed.
         */
        void seed_random(uint32 seed);

        uint32 get_uint32()
        {
            this->step();
            return uint32(x_ >> 17);
        }

        int32 get_int32()
        {
            this->step();
            return int32(x_ >> 17);
        }

        uint64 get_uint64()
        {
            return (uint64(this->get_uint32()) << 32) | this->get_uint32();
        }

        /**
         * \return Random value (0..N), but never N.
         */
        int32 get_int32(int32 max_exclusive)
        {
            assert(max_exclusive > 0);
            return this->get_int32() % max_exclusive;
        }

        /**
         * \return Random value (0..1), but never 1.0.
         */
        double get_double()
        {
            return double(this->get_int32()) / 0x80000000;
        }

        /**
         * \return Random value (0..1), but never 1.0.
         */
        float get_float()
        {
            return (float)this->get_int32() / 0x80000000;
        }

        float2 get_float2()
        {
            return float2(get_float(), get_float());
        }
        float3 get_float3()
        {
            return float3(get_float(), get_float(), get_float());
        }
        /**
         * Compute uniformly distributed barycentric coordinates.
         */
        float3 get_barycentric_coordinates()
        {
            float rand1 = this->get_float();
            float rand2 = this->get_float();

            if (rand1 + rand2 > 1.0f) {
                rand1 = 1.0f - rand1;
                rand2 = 1.0f - rand2;
            }

            return float3(rand1, rand2, 1.0f - rand1 - rand2);
        }

        /**
         * Round value to the next integer randomly.
         * 4.9f is more likely to round to 5 than 4.6f.
         */
        int round_probabilistic(float x);

        float2 get_unit_float2();
        float3 get_unit_float3();
        /**
         * Generate a random point inside the given triangle.
         */
        float2 get_triangle_sample(float2 v1, float2 v2, float2 v3);
        float3 get_triangle_sample_3d(float3 v1, float3 v2, float3 v3);
        /**
         * Simulate getting \a n random values.
         */
        void skip(int64 n)
        {
            while (n--) {
                this->step();
            }
        }

    private:
        void step()
        {
            constexpr uint64 multiplier = 0x5DEECE66Dll;
            constexpr uint64 addend = 0xB;
            constexpr uint64 mask = 0x0000FFFFFFFFFFFFll;

            x_ = (multiplier * x_ + addend) & mask;
        }
    };

}  // namespace blender