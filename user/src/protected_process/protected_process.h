#pragma once
#define PROTECTED_PROCESS_NAME "darken-anticheat-protected-process.exe"

namespace protected_process
{
	void initialise(), unload();

	inline unsigned long long process_id = 0ull;
}
