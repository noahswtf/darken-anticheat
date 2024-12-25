#pragma once
#include <string>
#include <guiddef.h>

namespace utilities
{
	namespace datatype
	{
		namespace guid
		{
			std::string to_string(GUID guid);
		}

		namespace uint32
		{
			std::string to_hexadecimal_string(std::uint32_t integral_number);
		}
	}
}
