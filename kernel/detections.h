#pragma once
#include "structures.h"
#include "types.h"

namespace detections
{
	namespace process
	{
		e_error find_suspicious_modules(communication::s_call_info*& call_info);
	}
}
