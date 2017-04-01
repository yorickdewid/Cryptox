#pragma once

#include <string>

namespace Util
{

template<typename T>
static std::string Hex(T data, size_t len)
{
	constexpr const char lut[] = "0123456789abcdef";

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i) {
		const unsigned char c = data[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}

	return output;
}

template<typename T>
static std::string Hex(T data)
{
	constexpr const char lut[] = "0123456789abcdef";

	std::string output;
	output.reserve(2 * data.size());
	for (size_t i = 0; i < data.size(); ++i) {
		const unsigned char c = data[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}

	return output;
}

};
