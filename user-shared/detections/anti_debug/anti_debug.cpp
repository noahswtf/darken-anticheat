#include "anti_debug.h"
#include <Windows.h>
#include <winternl.h>

communication::e_detection_status detections::anti_debug::is_peb_debug_flag_present()
{
	PPEB process_peb = reinterpret_cast<PPEB>(__readgsqword(0x60));

	return process_peb->BeingDebugged == 1 ? communication::e_detection_status::flagged : communication::e_detection_status::clean;
}
