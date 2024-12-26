#include "hypervisor.h"
#include <driver/driver.h>

communication::e_detection_status detections::hypervisor::check_reserved_msr_usage()
{
    return driver::send_call(communication::e_control_code::check_reserved_msr_usage, { }).detection_status;
}
