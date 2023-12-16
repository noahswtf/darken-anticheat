#pragma once
#include "structures.h"
#include "communication_types.h"

namespace detections
{
	enum class e_error
	{
		success,
		error
	};

	namespace process
	{
		e_error find_suspicious_modules(communication::s_call_info*& call_info);
	}
}
