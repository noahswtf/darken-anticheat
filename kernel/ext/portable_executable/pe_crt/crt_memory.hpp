#pragma once

#include "crt_types.hpp"

namespace portable_executable::pe_crt
{
	void memcpy(void* destination, const void* source, pe_crt::size_t size);

	void memset(void* destination, pe_crt::uint8_t value, pe_crt::size_t size);
}