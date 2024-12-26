#include "reserved_msr_usage.h"
#include "../shared_data/shared_data.h"
#include "../utilities/ntkrnl.h"
#include "../offsets/offsets.h"
#include "../log.h"

#include <ntifs.h>
#include <intrin.h>

communication::e_detection_status hypervisor::check_reserved_msr_usage()
{
	// reserved ranges
	for (uint32_t i = 0x40000000u; i <= 0x4000FFFFu; i++)
	{
		__try
		{
			// if this doesn't throw an exception, then we are likely in a virtualized environment and these ranges are being used to communicate between host and guest
			__readmsr(i);

			// should ever not get here if msr read throws exception
			d_log("[darken-anticheat] found usage of reserved MSR (index: %x).\n", i);

			return communication::e_detection_status::flagged;
		}
		__except (1)
		{

		}
	}

	return communication::e_detection_status::clean;
}
