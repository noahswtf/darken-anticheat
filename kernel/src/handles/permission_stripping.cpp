#include "permission_stripping.h"
#include "../log.h"
#include <ntifs.h>

namespace handles
{
	namespace permission_stripping
	{
		void* callback_handle = nullptr;
	}
}

#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000

OB_PREOP_CALLBACK_STATUS pre_operation_detour(communication::s_protected_processes* protected_processes, POB_PRE_OPERATION_INFORMATION pre_operation_information)
{
	// context hasn't been initialised yet. todo: monitor if protected processes structure has been tampered

	if (protected_processes->anticheat_usermode_id == 0 && protected_processes->protected_process_id == 0)
	{
		return OB_PREOP_SUCCESS;
	}

	PEPROCESS current_process = PsGetCurrentProcess();
	PEPROCESS target_process = reinterpret_cast<PEPROCESS>(pre_operation_information->Object);

	if (current_process == nullptr || target_process == nullptr) // undefined behaviour
	{
		return OB_PREOP_SUCCESS;
	}

	if (current_process == target_process)
	{
		return OB_PREOP_SUCCESS;
	}

	uint64_t current_process_id = reinterpret_cast<uint64_t>(PsGetProcessId(current_process));
	uint64_t target_process_id = reinterpret_cast<uint64_t>(PsGetProcessId(target_process));

	if (target_process_id == protected_processes->anticheat_usermode_id || target_process_id == protected_processes->protected_process_id)
	{
		// todo: send flags when we get here and be able to differenciate between malicious creators or not
		// might have to monitor whitelisted processes too to prevent people abusing them by injecting dlls into them and then opening handles from there

		pre_operation_information->Operation == OB_OPERATION_HANDLE_CREATE ?
			pre_operation_information->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION) :
			pre_operation_information->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION);
	
		d_log("[darken-anticheat] blocked handle being opened to (process id: 0x%llx) from (process id: 0x%llx).\n", target_process_id, current_process_id);
	}

	return OB_PREOP_SUCCESS;
}

void post_operation_detour(void* registration_context, POB_POST_OPERATION_INFORMATION operation_information)
{
	UNREFERENCED_PARAMETER(registration_context);
	UNREFERENCED_PARAMETER(operation_information);
}

bool handles::permission_stripping::load()
{
	OB_OPERATION_REGISTRATION ob_operation_registration = { };

	ob_operation_registration.ObjectType = PsProcessType;
	ob_operation_registration.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	ob_operation_registration.PreOperation = reinterpret_cast<POB_PRE_OPERATION_CALLBACK>(pre_operation_detour);
	ob_operation_registration.PostOperation = post_operation_detour;

	OB_CALLBACK_REGISTRATION callback_registration = { };

	callback_registration.OperationRegistration = &ob_operation_registration;
	callback_registration.RegistrationContext = &protected_processes;
	callback_registration.Version = OB_FLT_REGISTRATION_VERSION;
	callback_registration.Altitude = RTL_CONSTANT_STRING(L"361337"); // 360000 - 389999: FSFilter Activity Monitor
	callback_registration.OperationRegistrationCount = 1;

	return NT_SUCCESS(ObRegisterCallbacks(&callback_registration, &callback_handle));
}

void handles::permission_stripping::unload()
{
	if (callback_handle != nullptr)
	{
		ObUnRegisterCallbacks(callback_handle);

		callback_handle = nullptr;
	}
}
