#include "offsets.h"
#include <ntifs.h>

extern "C" uint32_t NtBuildNumber;

bool offsets::load()
{
	uint32_t build_number = NtBuildNumber;

	if (26100 <= build_number) // windows 11 24h2 +
	{
		eprocess::unique_process_id = 0x1D0;
	}
	else if (19041 <= build_number) // Windows 10 2004 -> Windows 11 23H2
	{
		eprocess::unique_process_id = 0x440;
	}
	else if (15063 <= build_number) // Windows 10 1703 -> Windows 10 1909
	{
		eprocess::unique_process_id = 0x2E0;
	}
	else if (10240 <= build_number) // Windows 10 1507 -> Windows 10 1607
	{
		// compile-time values are already defined for this version range
	}
	else
	{
		// is before Windows 10 1507 (first release version of Windows 10)

		return false;
	}

	eprocess::active_process_links = eprocess::unique_process_id + 8;

	return true;
}
