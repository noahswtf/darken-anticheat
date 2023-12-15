// darken anticheat user mode
// this is the user mode process of the anticheat
// thisll communicate with the driver to protect the application
#include "driver/driver.h"
#include <iostream>

int main()
{
	driver::initialise();

	driver::s_call_info call_info = driver::s_call_info();
	call_info.control_code = driver::e_call_code::test;

	std::cout << (driver::send_call(call_info).response == driver::e_response::clean ? "success" : "error") << '\n';

	driver::clean_up();
	return 0;
}
