#pragma once
class NoiseGenerator final
{
public:
	float static PerlinNoise3D(physx::PxVec3 pos, float frequency, int nrOctaves, float lacunarity, float persistence, const std::vector<int>hashes, const std::vector<physx::PxVec3>& gradients);
	float static PerlinNoise1D(uint32_t seed, float frequency, int nrOctaves, float lacunarity, float persistence, const std::vector<int>hashes, const std::vector<float>& gradients);

private:
	static float Smooth(float t);
	static float Dot(physx::PxVec3 g, float x, float y, float z);

	float static Perlin1D(uint32_t seed, float frequency, const std::vector<int>hashes, const std::vector<float>& gradients);

	float static Perlin3D(physx::PxVec3, float frequency, const std::vector<int>hashes, const std::vector<physx::PxVec3>& gradients);
};

