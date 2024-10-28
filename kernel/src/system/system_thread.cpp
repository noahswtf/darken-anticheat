#include "system_thread.h"
#include "../utilities/ntkrnl.h"
#include "../offsets/offsets.h"
#include "../structures/kldr_data_table_entry.h"

#include <ntifs.h>

bool check_thread_address_in_module_callback(uint64_t current_module_info, void* context)
{
	_KLDR_DATA_TABLE_ENTRY* current_module = reinterpret_cast<_KLDR_DATA_TABLE_ENTRY*>(current_module_info);

	uint64_t win32_thread_start_address = *reinterpret_cast<uint64_t*>(context);

	uint64_t current_module_base_address = reinterpret_cast<uint64_t>(current_module->DllBase);
	uint64_t current_module_end_address = current_module_base_address + current_module->SizeOfImage;

	if (current_module_base_address < win32_thread_start_address && win32_thread_start_address <= current_module_end_address)
	{
		*reinterpret_cast<uint64_t*>(context) = 1337;

		return true;
	}

	return false;
}

#pragma warning(push)
#pragma warning(disable: 6387) // for some reason msvc is throwing a warning that current_thread_id could be 0, even when i added a check before executing PsLookupThreadByThreadId

communication::e_detection_status system::system_thread::is_suspicious_thread_present()
{
	uint64_t our_executing_thread = ntkrnl::get_current_thread();

	// todo: enumerate pspcid table to find threads 'manually'
	// with my testing on Windows 11 24H2, thread ids go past 0x3000 boundary
	// i believe that 0x4000 is the limit but feel free to make a change if its not
	for (uint64_t current_thread_id = 4; current_thread_id <= 0x4000; current_thread_id += 4)
	{
		uint64_t current_ethread = 0;

		// they can just remove it from pspcid table
		// and we won't be able to find their thread by these means anymore
		if (NT_SUCCESS(PsLookupThreadByThreadId(reinterpret_cast<void*>(current_thread_id), reinterpret_cast<PETHREAD*>(&current_ethread))) == false)
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

		if (current_thread_win32_start_address == 0) // yes, people do set it to 0 for some odd reason
		{
			return communication::e_detection_status::flagged;
		}

		uint64_t enumerate_system_modules_callback_context = current_thread_win32_start_address;

		ntkrnl::enumerate_system_modules(check_thread_address_in_module_callback, &enumerate_system_modules_callback_context);

		// will be set to 1337 if in a legimate module
		if (enumerate_system_modules_callback_context != 1337)
		{
			return communication::e_detection_status::flagged;
		}
	}

	return communication::e_detection_status::clean;
}

#pragma warning(pop)
