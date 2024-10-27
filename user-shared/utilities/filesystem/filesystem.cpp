#include "filesystem.h"
#include <filesystem>
#include <fstream>

std::vector<std::uint8_t> utilities::filesystem::read_file(std::wstring_view path)
{
	std::ifstream file(path.data(), std::ios::binary);

	if (file.is_open() == false)
	{
		return { };
	}

	return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(file), { });
}
