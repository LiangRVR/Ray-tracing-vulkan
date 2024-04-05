#pragma once
#include <bits/stdc++.h>
#include <glm/glm.hpp>
#include <limits>

namespace Utils
{
    uint32_t ConvertToRGBA(const glm::vec4 &color);

    struct pcg_state_setseq_64
    {
        uint64_t state;
        uint64_t inc;
    };
    typedef struct pcg_state_setseq_64 pcg32_random_t;

    uint32_t pcg32_random_r(pcg32_random_t *rng);

    void pcg32_srandom_r(pcg32_random_t *rng, uint64_t initstate, uint64_t initSeq);

    void pcg32_srandom(uint64_t seed, uint64_t seq);

    uint32_t UInt();

    float RandomFloat();

    glm::vec3 Vec3();

    glm::vec3 InUnitSphere();
    glm::vec3 UnitVector();
}
