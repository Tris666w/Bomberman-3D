#include "stdafx.h"
#include "NoiseGenerator.h"

//Source https://catlikecoding.com/unity/tutorials/noise/
float NoiseGenerator::PerlinNoise3D(physx::PxVec3 pos, float frequency, int nrOctaves, float lacunarity,
	float persistence, const std::vector<int>hashes, const std::vector<physx::PxVec3>& gradients)
{
	float sum = Perlin3D(pos, frequency, hashes, gradients);
	float amplitude = 1.f;
	float range = 1.f;
	for (int o = 1; o < nrOctaves; o++)
	{
		frequency *= lacunarity;
		amplitude *= persistence;
		range += amplitude;
		sum += Perlin3D(pos, frequency, hashes, gradients) * amplitude;
	}
	return sum / range;
}

float NoiseGenerator::PerlinNoise1D(uint32_t seed, float frequency, const int nrOctaves, float lacunarity, float persistence,
	const std::vector<int> hashes, const std::vector<float>& gradients)
{
	float sum = Perlin1D(seed, frequency, hashes, gradients);
	float amplitude = 1.f;
	float range = 1.f;
	for (int o = 1; o < nrOctaves; o++)
	{
		frequency *= lacunarity;
		amplitude *= persistence;
		range += amplitude;
		sum += Perlin1D(seed, frequency, hashes, gradients) * amplitude;
	}
	return sum / range;
}

float NoiseGenerator::Smooth(float t)
{
	return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
}

float NoiseGenerator::Dot(physx::PxVec3 g, float x, float y, float z)
{
	return g.x * x + g.y * y + g.z * z;
}

float NoiseGenerator::Perlin1D(uint32_t seed, float frequency, const std::vector<int> hashes,
	const std::vector<float>& gradients)
{
	if (hashes.size() <= 0 || gradients.size() <= 0)
	{
		Logger::LogError(L"NoiseGenerator::Perlin1D, Gradients and hashes should have at least size 1");
	}

	auto const hashMask = hashes.size() - 1;
	auto const gradientMask = gradients.size() - 1;

	auto const point = seed * frequency;
	unsigned int i0 = static_cast<int>(floor(point));
	float const t0 = point - static_cast<float>(i0);
	float const t1 = t0 - 1.f;
	i0 &= hashMask;
	unsigned int i1 = i0 + 1;
	if (i1 >= hashMask)
		i1 = 0;

	float const g0 = gradients[hashes[i0] & gradientMask];
	float const g1 = gradients[hashes[i1] & gradientMask];

	float const v0 = g0 * t0;
	float const v1 = g1 * t1;

	float const t = Smooth(t0);
	return Interpolate(v0, v1, t) * 2.f;
}

float NoiseGenerator::Perlin3D(physx::PxVec3 pos, float frequency, const std::vector<int>hashes, const std::vector<physx::PxVec3>& gradients)
{
	using namespace physx;
	auto const hashMask = hashes.size() - 1;
	auto const gradientMask = gradients.size() - 1;

	pos *= frequency;

	int ix0 = static_cast<int>(floor(pos.x));
	int iy0 = static_cast<int>(floor(pos.y));
	int iz0 = static_cast<int>(floor(pos.z));
	float const tx0 = pos.x - static_cast<float>(ix0);
	float const ty0 = pos.y - static_cast<float>(iy0);
	float const tz0 = pos.z - static_cast<float>(iz0);
	float const tx1 = tx0 - 1.f;
	float const ty1 = ty0 - 1.f;
	float const tz1 = tz0 - 1.f;
	ix0 &= hashMask;
	iy0 &= hashMask;
	iz0 &= hashMask;
	int const ix1 = ix0 + 1;
	int const iy1 = iy0 + 1;
	int const iz1 = iz0 + 1;

	int const h0 = hashes[ix0];
	int const h1 = hashes[ix1];
	int const h00 = hashes[h0 + iy0];
	int const h10 = hashes[h1 + iy0];
	int const h01 = hashes[h0 + iy1];
	int const h11 = hashes[h1 + iy1];

	PxVec3 const g000 = gradients[hashes[h00 + iz0] & gradientMask];
	PxVec3 const g100 = gradients[hashes[h10 + iz0] & gradientMask];
	PxVec3 const g010 = gradients[hashes[h01 + iz0] & gradientMask];
	PxVec3 const g110 = gradients[hashes[h11 + iz0] & gradientMask];
	PxVec3 const g001 = gradients[hashes[h00 + iz1] & gradientMask];
	PxVec3 const g101 = gradients[hashes[h10 + iz1] & gradientMask];
	PxVec3 const g011 = gradients[hashes[h01 + iz1] & gradientMask];
	PxVec3 const g111 = gradients[hashes[h11 + iz1] & gradientMask];

	float const v000 = Dot(g000, tx0, ty0, tz0);
	float const v100 = Dot(g100, tx1, ty0, tz0);
	float const v010 = Dot(g010, tx0, ty1, tz0);
	float const v110 = Dot(g110, tx1, ty1, tz0);
	float const v001 = Dot(g001, tx0, ty0, tz1);
	float const v101 = Dot(g101, tx1, ty0, tz1);
	float const v011 = Dot(g011, tx0, ty1, tz1);
	float const v111 = Dot(g111, tx1, ty1, tz1);

	float const tx = Smooth(tx0);
	float const ty = Smooth(ty0);
	float const tz = Smooth(tz0);

	return Interpolate(
		Interpolate(Interpolate(v000, v100, tx), Interpolate(v010, v110, tx), ty),
		Interpolate(Interpolate(v001, v101, tx), Interpolate(v011, v111, tx), ty),
		tz);
}
