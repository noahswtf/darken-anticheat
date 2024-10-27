#include <driver/driver.h>
#include <detections/detections.h>
#include "protected_process/protected_process.h"

#include <thread>
#include <chrono>
#include <iostream>

int main()
{
	std::cout << "[darken-anticheat] started usermode process of anticheat.\n";

	if (driver::load() == false)
	{
		std::cout << "[darken-anticheat] unable to load driver.\n";

		return false;
	}

	std::cout << "[darken-anticheat] loaded driver successfully.\n";

	if (protected_process::load() == false)
	{
		std::cout << "[darken-anticheat] unable to load protected process.\n";

		return false;
	}

	std::cout << "[darken-anticheat] loaded protected process successfully.\n";

	while (true)
	{
		d_check_detection(detections::anti_debug::is_peb_debug_flag_present);
		d_check_detection(detections::modules::local_process::is_unsigned_module_present);

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	driver::unload();

	return 0;
}
