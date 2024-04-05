#include "Utils.h"

namespace Utils
{
    uint32_t ConvertToRGBA(const glm::vec4 &color)
    {
        glm::vec4 linearToGamma = glm::pow(color, glm::vec4(1.0f / 2.2f));
        uint8_t r = (uint8_t)(linearToGamma.r * 255.0f);
        uint8_t g = (uint8_t)(linearToGamma.g * 255.0f);
        uint8_t b = (uint8_t)(linearToGamma.b * 255.0f);
        uint8_t a = (uint8_t)(linearToGamma.a * 255.0f);

        uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }

    /* static uint32_t PCG_Hash(uint32_t input)
    {
        uint32_t state = input * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    static float RandomFloat(uint32_t &seed)
    {
        seed = PCG_Hash(seed);
        return (float)seed / (float)std::numeric_limits<uint32_t>::max();
    }
    static glm::vec3 InUnitSphere(uint32_t &seed)
    {
        return glm::normalize(glm::vec3(
            RandomFloat(seed) * 2.0f - 1.0f,
            RandomFloat(seed) * 2.0f - 1.0f,
            RandomFloat(seed) * 2.0f - 1.0f));
    } */
    pcg32_random_t pcg32_global = {0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL};
    bool initialized = false;

    uint32_t pcg32_random_r(pcg32_random_t *rng)
    {
        uint64_t oldState = rng->state;
        rng->state = oldState * 6364136223846793005ULL + rng->inc;
        uint32_t xorShifted = ((oldState >> 18u) ^ oldState) >> 27u;
        uint32_t rot = oldState >> 59u;
        return (xorShifted >> rot) | (xorShifted << ((-rot) & 31));
    }

    void pcg32_srandom_r(pcg32_random_t *rng, uint64_t initstate, uint64_t initSeq)
    {
        rng->state = 0U;
        rng->inc = (initSeq << 1u) | 1u;
        pcg32_random_r(rng);
        rng->state += initstate;
        pcg32_random_r(rng);
    }

    void pcg32_srandom(uint64_t seed, uint64_t seq)
    {
        pcg32_srandom_r(&pcg32_global, seed, seq);
    }

    void seedGenereator()
    {
        int rounds = 5;
        pcg32_srandom_r(&pcg32_global, time(NULL) ^ (intptr_t)&printf,
                        (intptr_t)&rounds);
    }

    uint32_t UInt()
    {
        if (!initialized)
        {
            seedGenereator();
            initialized = true;
        }
        uint32_t random_number = pcg32_random_r(&pcg32_global);
        return random_number;
    }

    float RandomFloat()
    {
        uint32_t random_number = UInt();
        float normalized_random = static_cast<float>(random_number) / static_cast<float>(std::numeric_limits<uint32_t>::max());
        return normalized_random;
    }

    glm::vec3 Vec3()
    {
        return glm::vec3(RandomFloat(), RandomFloat(), RandomFloat());
    }

    glm::vec3 InUnitSphere()
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

    glm::vec3 UnitVector()
    {
        return glm::normalize(InUnitSphere());
    }
}
