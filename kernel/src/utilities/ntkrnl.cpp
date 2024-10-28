#include "ntkrnl.h"
#include "../offsets/offsets.h"
#include "../structures/kldr_data_table_entry.h"

#include <ntifs.h>

extern "C" PLIST_ENTRY PsLoadedModuleList;

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

uint64_t ntkrnl::get_thread_eprocess(uint64_t ethread)
{
	return *reinterpret_cast<uint64_t*>(ethread + offsets::kthread::apc_state + offsets::kapc_state::process);
}

uint64_t ntkrnl::get_current_process()
{
	uint64_t current_thread = get_current_thread();

	return get_thread_eprocess(current_thread);
}

uint64_t ntkrnl::get_current_thread()
{
	return __readgsqword(0x188);
}

void ntkrnl::enumerate_system_modules(t_enumerate_modules_callback callback, void* context)
{
	for (PLIST_ENTRY current_list_entry = PsLoadedModuleList->Flink; current_list_entry != PsLoadedModuleList; current_list_entry = current_list_entry->Flink)
	{
		_KLDR_DATA_TABLE_ENTRY* current_module_info = CONTAINING_RECORD(current_list_entry, _KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		if (callback(context, reinterpret_cast<uint64_t>(current_module_info)) == true)
		{
			return;
		}
	}
}
