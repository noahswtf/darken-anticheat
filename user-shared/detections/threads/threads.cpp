#include "threads.h"
#include <driver/driver.h>

communication::e_detection_status detections::threads::is_suspicious_system_thread_present()
{
    return driver::send_call(communication::e_control_code::is_suspicious_system_thread_present, { }).detection_status;
}
