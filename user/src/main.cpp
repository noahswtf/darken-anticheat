#include "detections/detections.h"
#include "driver/driver.h"
#include "protected_process/protected_process.h"
#include "utilities/utilities.h"	
#include "log/log.h"	

#include <Windows.h>
#include <chrono>

int main()
{
	if (driver::load() == false)
	{
		d_dbg_error("failed to initialise kernel driver");
		system("pause");

		return 0;
	}

	if (protected_process::load() == false)
	{
		d_dbg_error("failed to initialise protected process");
		system("pause");

		return 0;
	}

	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

	while (true)
	{
		std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();

		// faster way of sleeping
		if (100ll <= std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count())
		{
			// this looks cluttered, just for debugging
			if (detections::system::find_suspicious_threads() == driver::e_response::clean
				&& detections::process::find_suspicious_modules(driver::host_process_id) == driver::e_response::clean
				&& detections::process::find_suspicious_threads(driver::host_process_id) == driver::e_response::clean
				&& detections::process::find_suspicious_modules(protected_process::process_id) == driver::e_response::clean
				&& detections::process::find_suspicious_threads(protected_process::process_id) == driver::e_response::clean
				&& detections::virtual_machine::check_in_virtual_machine() == driver::e_response::clean)
			{
				d_log("no flags");
			}
			else
			{
				d_log("currently flagging");
			}
		}
	}

	protected_process::unload();
	driver::unload();

	return 0;
}
