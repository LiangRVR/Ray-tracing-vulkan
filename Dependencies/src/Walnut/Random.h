#pragma once

#include <random>

#include <glm/glm.hpp>

namespace Walnut
{

	class Random
	{
	public:
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
		}

		static uint32_t UInt()
		{
			return s_Distribution(s_RandomEngine);
		}

		static uint32_t UInt(uint32_t min, uint32_t max)
		{
			return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static glm::vec3 Vec3()
		{
			return glm::vec3(Float(), Float(), Float());
		}

		static glm::vec3 Vec3(float min, float max)
		{
			return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
		}

		static glm::vec3 InUnitSphere()
		{
			return glm::normalize(Vec3(-1.0f, 1.0f));
		}

		/* static glm::vec3 InUnitSphere()
		{
			glm::vec3 p;
			do
			{
				p = Vec3(-1.0f, 1.0f);
			} while (glm::length(p) >= 1.0f);
			return p;
		} */

		/* static glm::vec3 InUnitSphere()
		{
			float theta = 2 * M_PI * Float(); // azimuthal angle
			float temp = 2 * Float() - 1;
			temp = temp < -1 ? -1 : temp > 1 ? 1
											 : temp; // ensure temp is within [-1, 1]
			float phi = acos(temp);					 // polar angle
			float r = cbrt(Float());				 // cube root of a random float to ensure uniform distribution

			float x = r * sin(phi) * cos(theta);
			float y = r * sin(phi) * sin(theta);
			float z = r * cos(phi);

			return glm::vec3(x, y, z);
		} */

		static glm::vec3 UnitVector()
		{
			return glm::normalize(InUnitSphere());
		}

	private:
		static thread_local std::mt19937 s_RandomEngine;
		static thread_local std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};

}
