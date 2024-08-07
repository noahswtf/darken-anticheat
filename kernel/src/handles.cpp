#include "handles.h"
#include "structures.h"

OB_PREOP_CALLBACK_STATUS pre_operation_detour(communication::s_protected_processes* registration_context, POB_PRE_OPERATION_INFORMATION operation_information)
{
	PEPROCESS target_process = reinterpret_cast<PEPROCESS>(operation_information->Object);
	unsigned long long target_process_id = reinterpret_cast<unsigned long long>(PsGetProcessId(target_process));

	PEPROCESS current_process = PsGetCurrentProcess();

	// we dont want to strip handles coming from system process, and we also only want to strip handles that are towards our protected processes
	if (current_process == target_process)
	{
		return OB_PREOP_SUCCESS;
	}

	if (operation_information->KernelHandle == 1)
	{
		return OB_PREOP_SUCCESS;
	}

	if (registration_context->user_mode_id != target_process_id && registration_context->protected_process_id != target_process_id)
	{
		return OB_PREOP_SUCCESS;
	}

	// strip permissions, 0x1000 is PROCESS_QUERY_LIMITED_INFORMATION
	operation_information->Operation == OB_OPERATION_HANDLE_CREATE ?
		operation_information->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | 0x1000) :
		operation_information->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | 0x1000);
	
#ifdef DEBUG
	DbgPrint("[darken-ac]: stripped permissions from handle being opened to the process: %llu from the process: %llu", target_process_id, reinterpret_cast<unsigned long long>(PsGetCurrentProcessId()));
#endif

	return OB_PREOP_SUCCESS;
}

// we dont need to do anything in the post
void post_operation_detour(void* registration_context, POB_POST_OPERATION_INFORMATION operation_information)
{
	UNREFERENCED_PARAMETER(registration_context);
	UNREFERENCED_PARAMETER(operation_information);
}

e_error handles::permission_stripping::initialise()
{
	OB_OPERATION_REGISTRATION ob_operation_registration[1] = { { } };

	ob_operation_registration[0].PreOperation = reinterpret_cast<POB_PRE_OPERATION_CALLBACK>(pre_operation_detour);
	ob_operation_registration[0].PostOperation = post_operation_detour;
	ob_operation_registration[0].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	ob_operation_registration[0].ObjectType = PsProcessType;

	OB_CALLBACK_REGISTRATION callback_registration = { 0 };
	callback_registration.OperationRegistration = ob_operation_registration;
	callback_registration.RegistrationContext = &shared::protected_processes; // passed in as context
	callback_registration.Version = OB_FLT_REGISTRATION_VERSION;
	callback_registration.Altitude = RTL_CONSTANT_STRING(L"1000");
	callback_registration.OperationRegistrationCount = 1U;

	return NT_SUCCESS(ObRegisterCallbacks(&callback_registration, &callback_handle)) ? e_error::success : e_error::error;
}

e_error handles::permission_stripping::unload()
{
	if (callback_handle)
	{
		ObUnRegisterCallbacks(callback_handle);
		ZwClose(callback_handle);

		return e_error::success;
	}

	return e_error::error;
}
