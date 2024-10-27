#include "driver.h"
#include <Windows.h>

namespace driver
{
	void* device_handle = nullptr;
}

bool driver::load()
{
	device_handle = CreateFileA("\\\\.\\darken-ac", GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE, 0UL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

	return device_handle != nullptr && device_handle != INVALID_HANDLE_VALUE;
}

void driver::unload()
{
	if (device_handle != nullptr && device_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(device_handle);

		device_handle = nullptr;
	}
}

communication::s_call_info driver::send_call(communication::e_control_code control_code, communication::s_call_info call_info)
{
	if (device_handle == nullptr || device_handle == INVALID_HANDLE_VALUE)
	{
		return { };
	}

	if (DeviceIoControl(device_handle, CTL_CODE(FILE_DEVICE_UNKNOWN, control_code, METHOD_BUFFERED, FILE_SPECIAL_ACCESS), &call_info, sizeof(communication::s_call_info), &call_info, sizeof(communication::s_call_info), nullptr, nullptr) == 0)
	{
		return { };
	}

	return call_info;
}
