#include "offsets.h"
#include "../context/context.h"
#include <ntifs.h>

bool offsets::load()
{
	context::s_context* context = context::get_decrypted();

	uint16_t build_number = *context->imports.nt_build_number;

	if (26100 <= build_number) // windows 11 24h2 +
	{
		eprocess::unique_process_id = 0x1D0;
		eprocess::section_base_address = 0x2B0;

		ethread::start_address = 0x4E0;
		ethread::win32_start_address = 0x560;
	}
	else if (22000 <= build_number) // Windows 11 Insider Preview -> Windows 11 23H2
	{
		eprocess::unique_process_id = 0x440;
		eprocess::section_base_address = 0x520;

		ethread::start_address = 0x4A0;
		ethread::win32_start_address = 0x520;
	}
	else if (19041 <= build_number) // Windows 10 2004 -> Windows 10 22H2
	{
		eprocess::unique_process_id = 0x440;
		eprocess::section_base_address = 0x520;

		ethread::start_address = 0x450;
		ethread::win32_start_address = 0x4D0;
	}
	else if (18362 <= build_number) // Windows 10 1903 -> Windows 10 1909
	{
		eprocess::unique_process_id = 0x2E0;
		eprocess::section_base_address = 0x3C8;

		ethread::start_address = 0x620;
		ethread::win32_start_address = 0x6A0;
	}
	else if (15063 <= build_number) // Windows 10 1703 -> Windows 10 1809
	{
		eprocess::unique_process_id = 0x2E0;

		ethread::start_address = 0x610;
		ethread::win32_start_address = 0x690;
	}
	else if (14393 <= build_number) // Windows 10 1607 only
	{
		ethread::start_address = 0x608;
		ethread::win32_start_address = 0x688;
	}
	else if (10240 <= build_number) // Windows 10 1507 -> Windows 10 1511
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
