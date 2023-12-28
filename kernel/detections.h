#pragma once
#include "structures.h"
#include "types.h"

namespace detections
{
	namespace process
	{
		e_error find_suspicious_modules(communication::s_call_info* call_info);
		e_error find_suspicious_threads(communication::s_call_info* call_info);
	}

	namespace system
	{
		e_error find_suspicious_threads(communication::s_call_info* call_info);
	}

	namespace virtual_machine
	{
		e_error check_msr_usage(communication::s_call_info* call_info);
	}
}
