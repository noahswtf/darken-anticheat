#include "detections.h"
#include <intrin.h>

extern "C" NTKERNELAPI NTSTATUS ZwQuerySystemInformation(ULONG, PVOID, ULONG, PULONG);
extern "C" NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS);

e_error detections::process::find_suspicious_modules(communication::s_call_info* call_info)
{
	call_info->response = communication::e_response::clean;

	PEPROCESS target_process = nullptr;

	if (!call_info->process_suspicious_modules_check.target_process_id || !NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(call_info->process_suspicious_modules_check.target_process_id), &target_process)))
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: invalid process id in [find_process_suspicious_modules].");
#endif

		return e_error::error;
	}

	KAPC_STATE apc;
	KeStackAttachProcess(target_process, &apc);

	PPEB process_peb = PsGetProcessPeb(target_process);

	if (!process_peb)
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: invalid process peb [find_process_suspicious_modules].");
#endif

		KeUnstackDetachProcess(&apc);
		ObfDereferenceObject(target_process);
		return e_error::error;
	}

	unsigned short modules_with_initial_base_address = 0U;

	PLIST_ENTRY list_head = &process_peb->Ldr->InMemoryOrderModuleList;

	for (PLIST_ENTRY current_list_entry = list_head->Flink; current_list_entry != list_head; current_list_entry = current_list_entry->Flink)
	{
		_LDR_DATA_TABLE_ENTRY* current_module = CONTAINING_RECORD(current_list_entry, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		bool is_suspicious = false;

		// we want to check if theyve just iterated through loaded modules and changed the base address to a false value
		if (current_module->DllBase == process_peb->ImageBaseAddress)
		{
			modules_with_initial_base_address++;
		}
		else
		{
			// really simple way to check if file is signed, but some windows dlls (such as apphelp.dll)
			// dont comply with this as they arent signed, so we should probably add some sanity checks for those

			_IMAGE_DOS_HEADER* dos_header = reinterpret_cast<_IMAGE_DOS_HEADER*>(current_module->DllBase);
			_IMAGE_NT_HEADERS64* nt_headers = reinterpret_cast<_IMAGE_NT_HEADERS64*>(reinterpret_cast<unsigned char*>(current_module->DllBase) + dos_header->e_lfanew);
		
			// if the address is null, then this means file is not signed
			if (!nt_headers->OptionalHeader.DataDirectory[4].VirtualAddress && wcscmp(current_module->BaseDllName.Buffer, L"apphelp.dll"))
			{
				is_suspicious = true;
			}
		}

		// there should only ever be 1
		if (2U <= modules_with_initial_base_address)
		{
			is_suspicious = true;
		}

		if (is_suspicious)
		{
			call_info->response = communication::e_response::flagged;
			call_info->flag_type = communication::e_flag_type::suspicious_module_in_process;

#ifdef DEBUG
			DbgPrint("[darken-ac]: found process suspicious module: %ls", current_module->BaseDllName.Buffer);
#endif
		}
	}

	KeUnstackDetachProcess(&apc);
	ObfDereferenceObject(target_process);

	return e_error::success;
}

e_error detections::process::find_suspicious_threads(communication::s_call_info* call_info)
{
	call_info->response = communication::e_response::clean;

	PEPROCESS pe_process;

	if (!call_info->process_suspicious_threads_check.target_process_id || !NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(call_info->process_suspicious_threads_check.target_process_id), &pe_process)))
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: invalid process id in [find_process_suspicious_threads].");
#endif

		return e_error::error;
	}

	_PEPROCESS process = reinterpret_cast<_PEPROCESS>(pe_process);

	KAPC_STATE apc = { };
	KeStackAttachProcess(pe_process, &apc);

	PPEB process_peb = PsGetProcessPeb(pe_process);
	
	if (!process_peb)
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: invalid process peb [find_process_suspicious_threads].");
#endif

		KeUnstackDetachProcess(&apc);
		ObfDereferenceObject(pe_process);
		return e_error::error;
	}

	PLIST_ENTRY thread_list_head = &process->ThreadListHead;
	
	for (PLIST_ENTRY thread_list = thread_list_head->Flink; thread_list != thread_list_head; thread_list = thread_list->Flink)
	{
		_PETHREAD current_thread = CONTAINING_RECORD(thread_list, _ETHREAD, ThreadListEntry);
		unsigned long long thread_start_address = reinterpret_cast<unsigned long long>(current_thread->StartAddress);

		bool is_suspicious = false;
		bool is_thread_inside_module = false;

		PLIST_ENTRY module_list_head = &process_peb->Ldr->InMemoryOrderModuleList;

		for (PLIST_ENTRY module_list = module_list_head->Flink; module_list != module_list_head; module_list = module_list->Flink)
		{
			_LDR_DATA_TABLE_ENTRY* current_module = CONTAINING_RECORD(module_list, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			unsigned long long module_base = reinterpret_cast<unsigned long long>(current_module->DllBase);
			unsigned long long module_size = static_cast<unsigned long long>(current_module->SizeOfImage);

			// check if thread win32 start address is inside module, if it isn't then we will then flag the user
			if (module_base <= thread_start_address && thread_start_address <= module_base + module_size)
			{
				is_thread_inside_module = true;
				break;
			}
		}

		if (is_thread_inside_module == false)
		{
			is_suspicious = true;
		}

		if (is_suspicious == true)
		{
			call_info->response = communication::e_response::flagged;
			call_info->flag_type = communication::e_flag_type::suspicious_thread_in_process;

#ifdef DEBUG
			DbgPrint("[darken-ac]: found process suspicious thread at address: 0x%llX", thread_start_address);
#endif
		}
	}

	KeUnstackDetachProcess(&apc);
	ObfDereferenceObject(pe_process);

	return e_error::success;
}

e_error is_thread_attached_to_process(unsigned long long process_id, _PETHREAD thread, bool* state)
{
	PEPROCESS process = nullptr;
	
	if (!NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(process_id), &process)))
	{
		return e_error::error;
	}

	KAPC_STATE apc_state = *reinterpret_cast<PKAPC_STATE>(reinterpret_cast<unsigned long long>(thread) + 0x98ull);

	*state = apc_state.Process == process;

	return e_error::success;
}

e_error detections::system::find_suspicious_threads(communication::s_call_info* call_info)
{
	call_info->response = communication::e_response::clean;

	// get the size of the process modules (as a whole)
	unsigned long size_of_process_modules = 0ul;
	ZwQuerySystemInformation(11ul, nullptr, size_of_process_modules, &size_of_process_modules);

	if (!size_of_process_modules)
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: unable to get size of system modules [find_system_suspicious_threads].");
#endif
		
		return e_error::error;
	}

	PRTL_PROCESS_MODULES process_modules = static_cast<PRTL_PROCESS_MODULES>(ExAllocatePool2(POOL_FLAG_NON_PAGED, size_of_process_modules, 'drkn'));

	if (!process_modules)
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: unable to allocate pool for process modules [find_system_suspicious_threads].");
#endif

		return e_error::error;
	}

	// fill pool with the process modules
	if (!NT_SUCCESS(ZwQuerySystemInformation(11ul, process_modules, size_of_process_modules, &size_of_process_modules)))
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: unable to fetch system modules [find_system_suspicious_threads].");
#endif

		ExFreePoolWithTag(process_modules, 'drkn');

		return e_error::error;
	}

	_PEPROCESS our_process = reinterpret_cast<_PEPROCESS>(IoGetCurrentProcess());

	PLIST_ENTRY thread_list_head = &our_process->ThreadListHead;

	for (PLIST_ENTRY thread_list = thread_list_head->Flink; thread_list != thread_list_head; thread_list = thread_list->Flink)
	{
		_PETHREAD current_thread = CONTAINING_RECORD(thread_list, _ETHREAD, ThreadListEntry);

		HANDLE current_thread_id = current_thread->Cid.UniqueThread;
		unsigned long long thread_start_address = reinterpret_cast<unsigned long long>(current_thread->Win32StartAddress);

		bool is_suspicious = false;
		bool is_thread_inside_module = false;

		// thread outside of valid module detection
		for (unsigned long i = 0ul; i < process_modules->NumberOfModules; i++)
		{
			unsigned long long module_base = reinterpret_cast<unsigned long long>(process_modules->Modules[i].ImageBase);
			unsigned long long module_size = static_cast<unsigned long long>(process_modules->Modules[i].ImageSize);

			// check if thread win32 start address is inside module, if it isn't then we will then flag the user
			if (module_base <= thread_start_address && thread_start_address <= module_base + module_size)
			{
				is_thread_inside_module = true;

				break;
			}
		}

		if (is_thread_inside_module == false)
		{
			is_suspicious = true;
		}

		// thread apc attached to one of our protected processes
		// reason why i thought this was a good addition is cause mmcopyvirtualmemory (widely used in cheats)
		// attaches to the process and this will now be detected
		bool thread_attached_to_usermode_process = false;
		bool thread_attached_to_protected_process = false;
		
		is_thread_attached_to_process(shared::protected_processes.user_mode_id, current_thread, &thread_attached_to_usermode_process);
		is_thread_attached_to_process(shared::protected_processes.protected_process_id, current_thread, &thread_attached_to_protected_process);

		if (thread_attached_to_usermode_process == true || thread_attached_to_protected_process == true)
		{
			is_suspicious = true;
		}

		if (is_suspicious == true)
		{
			call_info->response = communication::e_response::flagged;
			call_info->flag_type = communication::e_flag_type::suspicious_thread_in_system;

#ifdef DEBUG
			DbgPrint("[darken-ac]: found system/kernel suspicious thread with id '%llu' at address: 0x%llX", reinterpret_cast<unsigned long long>(current_thread_id), thread_start_address);
#endif
		}
	}

	ExFreePoolWithTag(process_modules, 'drkn');

	return e_error::success;
}

// credits to my good friend papstuc for telling me about this
e_error detections::virtual_machine::check_msr_usage(communication::s_call_info* call_info)
{
	call_info->response = communication::e_response::clean;

	// these ranges are specially reserved (as per intel docs)
	for (unsigned long i = 0x40000000UL; i <= 0x4000FFFFUL; i++)
	{
		__try
		{
			// used to communicate between host and virtual machine
			__readmsr(i);

			// if it doesnt throw an exception, then we're in a vm
#ifdef DEBUG
			DbgPrint("[darken-ac]: found msr usage, we may be running under a virtual machine");
#endif

			call_info->response = communication::e_response::flagged;
			return e_error::success;
		}
		__except (1)
		{

		}
	}

	return e_error::success;
}
