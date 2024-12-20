#pragma once
#include <communication_types.h>

namespace detections
{
	namespace patchguard
	{
		// should always return clean, if not, then means that io request was tampered or intercepted
		communication::e_detection_status trigger_bugcheck();
	}
}