#pragma once
#include <communication_types.h>

namespace driver
{
	bool load();
	void unload();

	communication::s_call_info send_call(communication::e_control_code control_code, communication::s_call_info);
}
