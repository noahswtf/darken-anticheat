#pragma once

enum class e_error
{
	success,
	error
};

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
		test = 0x1500,
		start_protections = 0x1501,
		check_suspicious_modules = 0x1502
	};

	struct s_suspicious_modules_check
	{
		unsigned long long target_process_id;
	};

	struct s_core_info
	{
		unsigned long long user_mode_process_id = 0ull, protected_process_process_id = 0ull;
	};

	struct s_call_info
	{
		e_call_code control_code = e_call_code::test;
		e_response response = e_response::flagged;
		e_flag_type flag_type = e_flag_type::none;

		s_core_info core_info;

		union
		{
			s_suspicious_modules_check suspicious_modules_check;
		};
	};

}
