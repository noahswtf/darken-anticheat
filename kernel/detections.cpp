#include "detections.h"

extern "C" NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS);

detections::e_error detections::process::find_suspicious_modules(communication::s_call_info*& call_info)
{
	call_info->response = communication::e_response::clean;

	if (!call_info->blacklisted_modules_check.whitelisted_modules || !call_info->blacklisted_modules_check.target_process_id)
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: no whitelisted modules or null process id in [find_suspicious_modules].");
#endif

		return e_error::error;
	}

	PEPROCESS target_process;

	if (!NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(call_info->blacklisted_modules_check.target_process_id), &target_process)))
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: invalid process id in [find_suspicious_modules].");
#endif

		return e_error::error;
	}

	KAPC_STATE apc;
	KeStackAttachProcess(target_process, &apc);

	PPEB process_peb = PsGetProcessPeb(target_process);

	if (!process_peb)
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: invalid process peb [find_suspicious_modules].");
#endif

		KeUnstackDetachProcess(&apc);
		return e_error::error;
	}

	unsigned short modules_with_initial_base_address = 0U;

	PLIST_ENTRY list_head = &process_peb->Ldr->InMemoryOrderModuleList;

	for (PLIST_ENTRY current_list_entry = list_head->Flink; current_list_entry != list_head; current_list_entry = current_list_entry->Flink)
	{
		_LDR_DATA_TABLE_ENTRY* current_module = CONTAINING_RECORD(current_list_entry, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		bool is_suspicious = false;

		if (current_module->DllBase == process_peb->ImageBaseAddress)
		{
			modules_with_initial_base_address++;
		}
		else
		{
			_IMAGE_DOS_HEADER* dos_header = reinterpret_cast<_IMAGE_DOS_HEADER*>(current_module->DllBase);
			_IMAGE_NT_HEADERS64* nt_headers = reinterpret_cast<_IMAGE_NT_HEADERS64*>(reinterpret_cast<unsigned char*>(current_module->DllBase) + dos_header->e_lfanew);
		
			// if the address is null, then this means file is not signed
			if (!nt_headers->OptionalHeader.DataDirectory[4].VirtualAddress)
			{
				is_suspicious = true;
			}
		}

		if (2U <= modules_with_initial_base_address)
		{
			is_suspicious = true;
		}

		if (is_suspicious)
		{
			call_info->response = communication::e_response::flagged;
			call_info->flag_type = communication::e_flag_type::suspicious_module_in_process;

			DbgPrint("[darken-ac]: found suspicious module: %ls", current_module->BaseDllName.Buffer);
		}
	}

	KeUnstackDetachProcess(&apc);

	return e_error::success;
}

