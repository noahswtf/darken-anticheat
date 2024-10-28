#pragma once

#ifdef _DARKEN_KERNEL
#include "generic_types.h"
#else
#include <cstdint>
#endif

namespace communication
{
	struct s_protected_processes
	{
		uint64_t anticheat_usermode_id;
		uint64_t protected_process_id;
	};

	enum e_control_code : uint32_t
	{
		test = 1,
		initialise_protected_processes,
		is_suspicious_system_thread_present
	};

	enum class e_detection_status : uint8_t
	{
		runtime_error,
		flagged,
		clean
	};

	struct s_call_info
	{
		e_detection_status detection_status = e_detection_status::runtime_error;

		union
		{
			s_protected_processes protected_processes = { };
		};
	};
}
