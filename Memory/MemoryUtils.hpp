#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>

// Returns a vector of all powers of two within minVal and maxVal inclusive
static std::vector<int> getPowersOfTwo(int minVal, int maxVal) {
	std::vector<int> powers;
	for (int i = 1; i <= maxVal; i *= 2) {
		if (i >= minVal) {
			powers.push_back(i);
		}
	}
	return powers;
}

// Picks a random power of two within minVal and maxVal inclusive
static int randomPowerOfTwo(int minVal, int maxVal) {
	auto powers = getPowersOfTwo(minVal, maxVal);
	if (powers.empty())
		throw std::runtime_error("No valid power-of-2 in range.");
	return powers[rand() % powers.size()];
}