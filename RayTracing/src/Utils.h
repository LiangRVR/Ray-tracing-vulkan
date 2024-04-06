#pragma once
#include <glm/glm.hpp>
#include <bits/stdc++.h>
#include <limits>

namespace Utils
{
    static uint32_t ConvertToRGBA(const glm::vec4 &color)
    {
        glm::vec4 linearToGamma = glm::pow(color, glm::vec4(1.0f / 2.2f));
        uint8_t r = (uint8_t)(linearToGamma.r * 255.0f);
        uint8_t g = (uint8_t)(linearToGamma.g * 255.0f);
        uint8_t b = (uint8_t)(linearToGamma.b * 255.0f);
        uint8_t a = (uint8_t)(linearToGamma.a * 255.0f);

        uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }

    struct pcg_state_setseq_64
    {
        uint64_t state;
        uint64_t inc;
    };
    typedef struct pcg_state_setseq_64 pcg32_random_t;

    thread_local static pcg_state_setseq_64 pcg32_global = {0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL};
    thread_local static bool initialized = false;

    static uint32_t pcg32_random_r(pcg32_random_t *rng)
    {
        uint64_t oldState = rng->state;
        rng->state = oldState * 6364136223846793005ULL + rng->inc;
        uint32_t xorShifted = ((oldState >> 18u) ^ oldState) >> 27u;
        uint32_t rot = oldState >> 59u;
        return (xorShifted >> rot) | (xorShifted << ((-rot) & 31));
    }

    static void pcg32_srandom_r(pcg32_random_t *rng, uint64_t initstate, uint64_t initSeq)
    {
        rng->state = 0U;
        rng->inc = (initSeq << 1u) | 1u;
        pcg32_random_r(rng);
        rng->state += initstate;
        pcg32_random_r(rng);
    }

    static void pcg32_srandom(uint64_t seed, uint64_t seq)
    {
        pcg32_srandom_r(&pcg32_global, seed, seq);
    }

    static void seedGenereator()
    {
        int rounds = 5;
        pcg32_srandom_r(&pcg32_global, time(NULL) ^ (intptr_t)&printf,
                        (intptr_t)&rounds);
    }

    static uint32_t UInt()
    {
        if (!initialized)
        {
            seedGenereator();
            initialized = true;
        }
        uint32_t random_number = pcg32_random_r(&pcg32_global);
        return random_number;
    }

    static float RandomFloat()
    {
        uint32_t random_number = UInt();
        float normalized_random = static_cast<float>(random_number) / static_cast<float>(std::numeric_limits<uint32_t>::max());
        return normalized_random;
    }

    static glm::vec3 Vec3()
    {
        return glm::vec3(RandomFloat(), RandomFloat(), RandomFloat());
    }

    static glm::vec3 InUnitSphere()
    {
        glm::vec3 rand = Vec3();
        float theta = rand.x * 2.0 * 3.14159265;
        float v = rand.y;
        float phi = acos(2.0 * v - 1.0);
        float r = pow(rand.z, 1.0 / 3.0);
        float x = r * sin(phi) * cos(theta);
        float y = r * sin(phi) * sin(theta);
        float z = r * cos(phi);
        return glm::vec3(x, y, z);
    }

    static glm::vec3 InUnitDisk()
    {
        glm::vec3 rand = InUnitSphere();
        return glm::vec3(rand.x, rand.y, 0.0);
    }

    static glm::vec3 UnitVector()
    {
        return glm::normalize(InUnitSphere());
    }
}
