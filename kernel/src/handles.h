#pragma once
#include "shared.h"

namespace handles
{
	namespace permission_stripping
	{
		e_error initialise();
		e_error unload();

		inline void* callback_handle = nullptr;
	}
}
