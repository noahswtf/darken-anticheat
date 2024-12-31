#pragma once
#include "../context/context.h"

namespace handles
{
	namespace permission_stripping
	{
		bool load(context::s_context* context);
		void unload();
	}
}
