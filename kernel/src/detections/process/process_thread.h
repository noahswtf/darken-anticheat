#pragma once
#include "../../context/context.h"

namespace process
{
	namespace process_thread
	{
		communication::e_detection_status is_suspicious_thread_present(context::s_context* context, communication::s_is_suspicious_process_thread_present is_suspicious_process_thread_present);
	}
}
