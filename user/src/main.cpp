#include "detections/detections.h"
#include "driver/driver.h"
#include "protected_process/protected_process.h"
#include "utilities/utilities.h"	
#include <iostream>

#include <Windows.h>
#include <chrono>

int main()
{
	driver::initialise();
	protected_process::initialise();

	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

	while (true)
	{
		std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();

		// faster way of sleeping
		if (100ULL <= std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count())
		{
			// this looks cluttered, just for debugging
			if (detections::system::find_suspicious_threads() == driver::e_response::clean
				&& detections::process::find_suspicious_modules(GetCurrentProcessId()) == driver::e_response::clean
				&& detections::process::find_suspicious_threads(GetCurrentProcessId()) == driver::e_response::clean
				&& detections::process::find_suspicious_modules(protected_process::process_id) == driver::e_response::clean
				&& detections::process::find_suspicious_threads(protected_process::process_id) == driver::e_response::clean
				&& detections::virtual_machine::check_in_virtual_machine() == driver::e_response::clean
				)
			{
				std::cout << "  [darken-anticheat] no flags.\n";
			}
			else
			{
				std::cout << "  [darken-anticheat] currently flagging.\n";
			}
		}
	}

	protected_process::unload();
	driver::unload();
	return 0;
}
