#pragma once
#define PROTECTED_PROCESS_NAME "darken-anticheat-protected-process.exe"

namespace protected_process
{
	bool load();
	void unload();

	inline unsigned long long process_id = 0ull;
}
