#include "detections.h"

extern "C" NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS);
extern "C" NTKERNELAPI void* PsGetThreadWin32Thread(PETHREAD);

e_error detections::process::find_suspicious_modules(communication::s_call_info*& call_info)
{
	call_info->response = communication::e_response::clean;

	PEPROCESS target_process;

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

			DbgPrint("[darken-ac]: found process suspicious module: %ls", current_module->BaseDllName.Buffer);
		}
	}

	KeUnstackDetachProcess(&apc);
	ObfDereferenceObject(target_process);

	return e_error::success;
}

e_error detections::process::find_suspicious_threads(communication::s_call_info*& call_info)
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

	KAPC_STATE apc;
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
		unsigned long long thread_start_address = reinterpret_cast<unsigned long long>(current_thread->Win32StartAddress);

		bool is_suspicious = false;

		PLIST_ENTRY module_list_head = &process_peb->Ldr->InMemoryOrderModuleList;
		for (PLIST_ENTRY module_list = module_list_head->Flink; module_list != module_list_head; module_list = module_list->Flink)
		{
			_LDR_DATA_TABLE_ENTRY* current_module = CONTAINING_RECORD(module_list, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			unsigned long long module_base = reinterpret_cast<unsigned long long>(current_module->DllBase);
			unsigned long long module_size = static_cast<unsigned long long>(current_module->SizeOfImage);

			if (!thread_start_address || (module_base < thread_start_address && thread_start_address < module_base + module_size))
			{
				is_suspicious = true;
			}
		}

		if (!is_suspicious)
		{
			call_info->response = communication::e_response::flagged;
			call_info->flag_type = communication::e_flag_type::suspicious_thread;

			DbgPrint("[darken-ac]: found process suspicious thread at address: 0x%p", current_thread->StartAddress);
		}
	}

	KeUnstackDetachProcess(&apc);
	ObfDereferenceObject(pe_process);

	return e_error::success;
}
