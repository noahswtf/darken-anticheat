#include "detections.h"
#include "../driver/driver.h"

driver::e_response detections::process::find_suspicious_modules(unsigned long long process_id)
{
	driver::s_call_info call_info = driver::s_call_info();
	call_info.control_code = driver::e_call_code::check_process_suspicious_modules;
	call_info.process_suspicious_modules_check.target_process_id = process_id;

	return driver::send_call(call_info).response;
}

driver::e_response detections::process::find_suspicious_threads(unsigned long long process_id)
{
	driver::s_call_info call_info = driver::s_call_info();
	call_info.control_code = driver::e_call_code::check_process_suspicious_threads;
	call_info.process_suspicious_threads_check.target_process_id = process_id;

	return driver::send_call(call_info).response;
}

driver::e_response detections::system::find_suspicious_threads()
{
	driver::s_call_info call_info = driver::s_call_info();
	call_info.control_code = driver::e_call_code::check_system_suspicious_threads;
	
	return driver::send_call(call_info).response;
}
