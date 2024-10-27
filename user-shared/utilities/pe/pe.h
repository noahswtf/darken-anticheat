#pragma once
#include <string>

namespace utilities
{
	namespace pe
	{
		bool is_digitally_signed(std::wstring_view binary_path);
		bool has_pdb_on_microsoft_symbol_server(std::wstring_view binary_path);
	}
}
