#pragma once
#include <generic_types.h>

namespace ntkrnl
{
	uint64_t get_eprocess(uint64_t target_process_id);
	uint64_t get_process_id(uint64_t eprocess);

	uint64_t get_thread_eprocess(uint64_t ethread);
	uint64_t get_current_process();
	uint64_t get_current_thread();

	// return value signals if needs to continue enumerating
	// current_module_info will contain a pointer to: _KLDR_DATA_TABLE_ENTRY
	typedef bool(*t_enumerate_modules_callback)(uint64_t current_module_info);

	void enumerate_system_modules(uint64_t eprocess, t_enumerate_modules_callback callback);
}
