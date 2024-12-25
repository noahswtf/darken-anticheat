#pragma once
#include <cstdint>

namespace protected_process
{
	bool load();

	inline std::uint64_t process_id = 0;
}
