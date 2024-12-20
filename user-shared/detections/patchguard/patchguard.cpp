#include "patchguard.h"
#include <driver/driver.h>

communication::e_detection_status detections::patchguard::trigger_bugcheck()
{
    return driver::send_call(communication::e_control_code::trigger_patchguard_bugcheck, { }).detection_status;
}
