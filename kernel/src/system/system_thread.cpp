#include "system_thread.h"
#include "../utilities/ntkrnl.h"

#include <ntifs.h>

communication::e_detection_status system::system_thread::is_suspicious_thread_present()
{
	uint64_t system_eprocess = ntkrnl::get_eprocess(4);

	if (system_eprocess == 0)
	{
		return communication::e_detection_status::runtime_error;
	}

	return communication::e_detection_status::clean;
}
