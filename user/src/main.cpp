#include "driver/driver.h"
#include "protected_process/process.h"
#include "utilities/utilities.h"	
#include <iostream>

#include <Windows.h>

int main()
{
	driver::initialise();
	process::initialise();

	while (true)
	{
		driver::s_call_info call_info = driver::s_call_info();
		call_info.control_code = driver::e_call_code::check_process_suspicious_threads;
		call_info.process_suspicious_threads_check.target_process_id = GetCurrentProcessId();

		std::cout << (driver::send_call(call_info).response == driver::e_response::clean ? "no flags" : "flagged") << '\n';

		Sleep(100);
	}

	driver::unload();
	return 0;
}
