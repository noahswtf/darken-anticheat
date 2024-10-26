#pragma once
#include <cstdint>

enum class e_detection_status : std::uint8_t
{
	runtime_error,
	flagged,
	clean
};
