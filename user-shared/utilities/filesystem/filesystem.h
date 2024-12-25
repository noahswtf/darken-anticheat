#pragma once
#include <string>
#include <vector>

namespace utilities
{
	namespace filesystem
	{
		std::vector<std::uint8_t> read_file(std::wstring_view path);
	}
}
