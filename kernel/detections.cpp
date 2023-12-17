#include "detections.h"

extern "C" NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS);

e_error detections::process::find_suspicious_modules(communication::s_call_info*& call_info)
{
	call_info->response = communication::e_response::clean;

	PEPROCESS target_process;

	if (!call_info->suspicious_modules_check.target_process_id || !NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(call_info->suspicious_modules_check.target_process_id), &target_process)))
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

		// we want to check if theyve just iterated through loaded modules and changed the base address to a false value
		if (current_module->DllBase == process_peb->ImageBaseAddress)
		{
			modules_with_initial_base_address++;
		}
		else
		{
			// really simple way to check if file is signed, but some windows dlls (such as apphelp.dll)
			// dont comply with this as they arent signed, so we should probably add some sanity checks

			_IMAGE_DOS_HEADER* dos_header = reinterpret_cast<_IMAGE_DOS_HEADER*>(current_module->DllBase);
			_IMAGE_NT_HEADERS64* nt_headers = reinterpret_cast<_IMAGE_NT_HEADERS64*>(reinterpret_cast<unsigned char*>(current_module->DllBase) + dos_header->e_lfanew);
		
			// if the address is null, then this means file is not signed
			if (!nt_headers->OptionalHeader.DataDirectory[4].VirtualAddress)
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

			DbgPrint("[darken-ac]: found suspicious module: %ls", current_module->BaseDllName.Buffer);
		}
	}

	KeUnstackDetachProcess(&apc);

	return e_error::success;
}
