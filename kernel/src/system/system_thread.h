#pragma once
#include "../context/context.h"

namespace system
{
	namespace system_thread
	{
		communication::e_detection_status is_suspicious_thread_present(context::s_context* context);
	}
}
