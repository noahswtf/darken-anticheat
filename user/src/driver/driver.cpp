#include "driver.h"
#include "../log/log.h"
#include <Windows.h>

#define IO_CODE(Code) CTL_CODE(FILE_DEVICE_UNKNOWN, Code, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

namespace driver
{
	inline HANDLE handle = nullptr;
}

bool driver::load()
{
	handle = CreateFileA("\\\\.\\darken-ac", GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE, 0UL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

	if (handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	host_process_id = static_cast<unsigned long long>(GetCurrentProcessId());

	return true;
}

void driver::unload()
{
	CloseHandle(handle);
}

void driver::initialise_protected_processes(s_protected_processes protected_processes)
{
	s_call_info call_info;
	call_info.control_code = e_call_code::initialise_protected_processes;
	call_info.protected_processes = protected_processes;

	send_call(call_info);
}

driver::s_call_info driver::send_call(s_call_info call_info)
{
	DeviceIoControl(handle, CTL_CODE(FILE_DEVICE_UNKNOWN, static_cast<unsigned long>(call_info.control_code), METHOD_BUFFERED, FILE_SPECIAL_ACCESS), &call_info, sizeof(s_call_info), &call_info, sizeof(s_call_info), nullptr, nullptr);

	return call_info;
}
