#pragma once

namespace communication
{
	enum class e_response : unsigned long long
	{
		flagged,
		clean
	};

	enum class e_flag_type
	{
		none,
		suspicious_module_in_process
	};

	enum class e_call_code : unsigned long
	{
		test = 0x1500
	};

	struct s_blacklisted_modules_check
	{
		unsigned long long target_process_id = 0ull;
		wchar_t* whitelisted_modules[4];
	};

	struct s_call_info
	{
		e_call_code control_code = e_call_code::test;
		e_response response = e_response::flagged;
		e_flag_type flag_type = e_flag_type::none;

		union
		{
			s_blacklisted_modules_check blacklisted_modules_check;
		};
	};
}
