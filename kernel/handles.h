#pragma once
#include "types.h"

namespace handles
{
	namespace permission_stripping
	{
		struct s_process_ids
		{
			unsigned long long user_mode = 0ull, protected_process = 0ull;
		};

		e_error initialise();
		e_error unload();

		inline void* callback_handle = nullptr;
		inline communication::s_core_info process_ids = { };
	}
}
