#pragma once
#include <generic_types.h>

namespace ntkrnl
{
	uint64_t get_eprocess(uint64_t target_process_id);
	uint64_t get_process_id(uint64_t eprocess);

	uint64_t get_current_process();
	uint64_t get_current_thread();
}
