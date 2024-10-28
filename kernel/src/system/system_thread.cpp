#include "system_thread.h"
#include "../utilities/ntkrnl.h"
#include "../offsets/offsets.h"

#include <ntifs.h>

communication::e_detection_status system::system_thread::is_suspicious_thread_present()
{
	uint64_t our_executing_thread = ntkrnl::get_current_thread();

	// todo: enumerate pspcid table to find threads 'manually'
	for (uint64_t current_thread_id = 4; current_thread_id < 0x3000; current_thread_id++)
	{
		uint64_t current_ethread = 0;

		// they can just remove it from pspcid table
		// and we won't be able to find their thread by these means anymore
		if (NT_SUCCESS(PsLookupThreadByThreadId(reinterpret_cast<HANDLE>(current_thread_id), reinterpret_cast<PETHREAD*>(&current_ethread))) == false)
		{
			continue;
		}

		if (our_executing_thread == current_ethread)
		{
			continue;
		}

		uint64_t current_thread_eprocess = ntkrnl::get_thread_eprocess(current_ethread);
		uint64_t current_thread_process_id = *reinterpret_cast<uint64_t*>(current_thread_eprocess + offsets::eprocess::unique_process_id);

		if (current_thread_process_id != 4)
		{
			continue;
		}

		// check if Win32StartAddress is outside of any legitimate kernel module.
		// can be bypassed by changing the value in the ETHREAD struct
		// or using a (jmp rcx) gadget which is in legitimate module and passing the real start address as context

		uint64_t current_thread_win32_start_address = *reinterpret_cast<uint64_t*>(current_ethread + offsets::ethread::win32_start_address);

		if (current_thread_win32_start_address == 0) // yes, people do set it to 0
		{
			return communication::e_detection_status::flagged;
		}


	}

	return communication::e_detection_status::clean;
}
