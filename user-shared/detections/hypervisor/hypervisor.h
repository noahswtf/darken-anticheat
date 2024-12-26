#pragma once
#include <communication_types.h>

namespace detections
{
	namespace hypervisor
	{
		communication::e_detection_status check_reserved_msr_usage();
	}
}