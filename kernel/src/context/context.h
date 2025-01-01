#pragma once
#include <communication_types.h>
#include "../imports/imports_def.h"

#define d_pool_tag 'drac'

namespace context
{
	bool load();
	void unload();

	// we store pointers for structures that may need to be rewritten to (eg: protected processes)
	// stuff such as imports will never need to be written though
	struct s_context
	{
		communication::s_protected_processes protected_processes;
		imports::s_imports imports;

		uint64_t ntoskrnl_base_address = 0;
		uint64_t initial_system_process = 0;
	};

	// holds pointer to encrypted s_context
	// todo: replace xor encryption with something stronger
	inline uint64_t encrypted_context_pointer = 0;

	s_context* get_decrypted();
}
