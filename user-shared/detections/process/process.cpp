#include "process.h"
#include <driver/driver.h>

communication::e_detection_status detections::process::is_suspicious_thread_present(std::uint64_t process_id)
{
    communication::s_call_info call_info = { };

    call_info.is_suspicious_process_thread_present.process_id = process_id;

    return driver::send_call(communication::e_control_code::is_suspicious_process_thread_present, call_info).detection_status;
}
