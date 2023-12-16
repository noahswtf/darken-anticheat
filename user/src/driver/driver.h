#pragma once

namespace driver
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
		test = 0x1500,
		check_suspicious_modules = 0x1501
	};

	struct s_suspicious_modules_check
	{
		unsigned long long target_process_id;
	};

	struct s_call_info
	{
		e_call_code control_code = e_call_code::test;
		e_response response = e_response::flagged;
		e_flag_type flag_type = e_flag_type::none;

		union
		{
			s_suspicious_modules_check suspicious_modules_check;
		};
	};

	s_call_info send_call(s_call_info call_info);

	void initialise(), clean_up();

	inline void* handle = nullptr;
}