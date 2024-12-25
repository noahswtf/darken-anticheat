#pragma once
#include <communication_types.h>

namespace detections
{
	namespace process
	{
		communication::e_detection_status is_suspicious_thread_present(std::uint64_t process_id);
	}
}
