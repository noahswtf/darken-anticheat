#include <detections/detections.h>

#include <thread>
#include <chrono>
#include <iostream>

int main()
{
	std::cout << "[darken-anticheat] - started usermode process of anticheat\n";

	while (true)
	{
		d_check_detection(detections::anti_debug::is_peb_debug_flag_present);

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	return 0;
}
