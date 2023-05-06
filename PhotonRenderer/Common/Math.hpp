#pragma once
#include <random>
#define POWI(base, exp) (int)powf((float)(base), (float)(exp))
namespace photon::math {
static int NextPowerOfTwo(int number) {
  --number;  // Decrease the number by 1

  // Bitwise OR to set all bits after the most significant bit
  for (size_t i = 1; i < sizeof(unsigned int) * 8; i <<= 1) {
    number |= number >> i;
  }

  ++number;  // Increase the number by 1 to get the next power of 2

  return number;
}

// Generate a random float in the given range [min, max]
static float RandomFloat(float min, float max) {
  // Create a random number generator
  std::random_device rd;
  std::mt19937 gen(rd());

  // Create a distribution for floats in the range [min, max]
  std::uniform_real_distribution<float> dis(min, max);

  // Generate and return a random float
  return dis(gen);
}
}  // namespace photon::math