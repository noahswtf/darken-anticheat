#pragma once
#include "../driver/response.h"

// similar structure to km header
namespace detections
{
	namespace process
	{
		driver::e_response find_suspicious_modules(unsigned long long process_id);
		driver::e_response find_suspicious_threads(unsigned long long process_id);
	}

	namespace system
	{
		driver::e_response find_suspicious_threads();
	}

	namespace virtual_machine
	{
		driver::e_response check_in_virtual_machine();
	}
}
