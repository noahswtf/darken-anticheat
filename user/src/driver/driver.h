#pragma once

namespace driver
{
	enum class e_response : unsigned long long
	{
		flagged,
		clean,
		runtime_error
	};

	enum class e_flag_type
	{
		none,
		suspicious_module_in_process,
		suspicious_thread_in_process,
		suspicious_thread_in_system
	};

	enum class e_call_code : unsigned long
	{
		test = 0x1500,
		start_protections = 0x1501,
		check_process_suspicious_modules = 0x1502,
		check_process_suspicious_threads = 0x1503,
		check_system_suspicious_threads = 0x1504
	};

	struct s_process_suspicious_modules_check
	{
		unsigned long long target_process_id;
	};

	struct s_process_suspicious_threads_check
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
			s_process_suspicious_modules_check process_suspicious_modules_check;
			s_process_suspicious_threads_check process_suspicious_threads_check;
		};
	};

	s_call_info send_call(s_call_info call_info);

	void initialise(), unload(), start_protections(s_core_info core_info);

	inline void* handle = nullptr;
}
