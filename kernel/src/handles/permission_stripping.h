#pragma once
#include <communication_types.h>

namespace handles
{
	namespace permission_stripping
	{
		bool load();
		void unload();

		inline communication::s_protected_processes protected_processes = { };
	}
}
