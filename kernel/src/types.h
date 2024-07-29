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
		initialise_protected_processes,
		check_process_suspicious_modules,
		check_process_suspicious_threads,
		check_system_suspicious_threads,
		check_in_virtual_machine
	};

	struct s_protected_processes
	{
		unsigned long long user_mode_id = 0ull, protected_process_id = 0ull;
	};

	struct s_process_suspicious_modules_check
	{
		unsigned long long target_process_id;
	};

	struct s_process_suspicious_threads_check
	{
		unsigned long long target_process_id;
	};

	struct s_call_info
	{
		e_call_code control_code = e_call_code::test;
		e_response response = e_response::flagged;
		e_flag_type flag_type = e_flag_type::none;

		s_process_suspicious_modules_check process_suspicious_modules_check;
		s_process_suspicious_threads_check process_suspicious_threads_check;
		s_protected_processes protected_processes;
	};
}
