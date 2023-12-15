#include "driver.h"
#include <Windows.h>

#include <iostream>

#define IO_CODE(Code) CTL_CODE(FILE_DEVICE_UNKNOWN, Code, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

void driver::initialise()
{
	handle = CreateFileA("\\\\.\\darken-ac", GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE, 0UL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

	if (handle == INVALID_HANDLE_VALUE)
	{
		std::cout << "handle to driver invalid\n";
	}
}

void driver::clean_up()
{
	CloseHandle(handle);
}

driver::s_call_info driver::send_call(s_call_info call_info)
{
	DeviceIoControl(handle, CTL_CODE(FILE_DEVICE_UNKNOWN, static_cast<unsigned long>(call_info.control_code), METHOD_BUFFERED, FILE_SPECIAL_ACCESS), &call_info, sizeof(s_call_info), &call_info, sizeof(s_call_info), nullptr, nullptr);

	return call_info;
}
