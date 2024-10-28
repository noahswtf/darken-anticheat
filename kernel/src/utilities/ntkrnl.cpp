#include "ntkrnl.h"
#include "../offsets/offsets.h"

#include <ntifs.h>

uint64_t ntkrnl::get_eprocess(uint64_t target_process_id)
{
	uint64_t initial_system_process = reinterpret_cast<uint64_t>(PsInitialSystemProcess);

	if (target_process_id == 4)
	{
		return initial_system_process;
	}

	PLIST_ENTRY list_head = reinterpret_cast<PLIST_ENTRY>(initial_system_process + offsets::eprocess::active_process_links);

	for (PLIST_ENTRY current_entry = list_head->Flink; current_entry != list_head; current_entry = current_entry->Flink)
	{
		uint64_t current_entry_eprocess = reinterpret_cast<uint64_t>(current_entry) - offsets::eprocess::active_process_links;
		uint64_t current_entry_process_id = *reinterpret_cast<uint64_t*>(current_entry_eprocess + offsets::eprocess::unique_process_id);

		if (current_entry_process_id == target_process_id)
		{
			return current_entry_eprocess;
		}
	}

	return 0;
}

uint64_t ntkrnl::get_process_id(uint64_t eprocess)
{
	return *reinterpret_cast<uint64_t*>(eprocess + offsets::eprocess::unique_process_id);
}

uint64_t ntkrnl::get_current_process()
{
	uint64_t current_thread = get_current_thread();

	return *reinterpret_cast<uint64_t*>(current_thread + offsets::kthread::apc_state + offsets::kapc_state::process);
}

uint64_t ntkrnl::get_current_thread()
{
	return __readgsqword(0x188);
}
