#pragma once
#include "generic_types.h"

typedef void(__stdcall* t_iof_complete_request)(void* irp, int8_t priority_boost);
typedef uint64_t(__stdcall* t_ex_allocate_pool_2)(uint64_t pool_flags, uint64_t number_of_bytes, uint32_t tag);
typedef uint64_t(__stdcall* t_ob_register_callbacks)(uint64_t pool_flags, uint64_t number_of_bytes, uint32_t tag);

namespace imports
{
	struct s_imports
	{
		uint16_t* nt_build_number;
		t_iof_complete_request iof_complete_request;
	};
}
