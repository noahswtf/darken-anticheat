#pragma once

namespace driver
{
	enum class e_response : unsigned long long
	{
		flagged,
		clean
	};

	enum class e_call_code : unsigned long
	{
		test = 0x1500
	};

	struct s_call_info
	{
		e_call_code control_code = e_call_code::test;
		e_response response = e_response::flagged;
	};

	s_call_info send_call(s_call_info call_info);

	void initialise(), clean_up();

	inline void* handle = nullptr;
}
