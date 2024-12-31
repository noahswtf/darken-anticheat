#include "handles/permission_stripping.h"
#include "hypervisor/reserved_msr_usage.h"
#include "system/system_thread.h"
#include "process/process_thread.h"
#include "context/context.h"
#include "patchguard/patchguard.h"
#include "utilities/ntkrnl.h"
#include "imports/imports.h"
#include "offsets/offsets.h"
#include "log.h"

#include <ntifs.h>

#define d_control_code(code) CTL_CODE(FILE_DEVICE_UNKNOWN, code, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

namespace driver_info
{
	UNICODE_STRING device_name = RTL_CONSTANT_STRING(L"\\Device\\darken-ac");
	UNICODE_STRING device_symbolic_name = RTL_CONSTANT_STRING(L"\\DosDevices\\darken-ac");
}

NTSTATUS ioctl_manage_call(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	irp->IoStatus.Information = 0ull;
	irp->IoStatus.Status = STATUS_SUCCESS;

	context::s_context* context = context::get_decrypted();

	context->imports.iof_complete_request(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS ioctl_call_processor(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	context::s_context* context = context::get_decrypted();

	// IoGetCurrentIrpStackLocation is force inlined
	uint32_t code = IoGetCurrentIrpStackLocation(irp)->Parameters.DeviceIoControl.IoControlCode;

	communication::s_call_info* call_info = reinterpret_cast<communication::s_call_info*>(irp->AssociatedIrp.SystemBuffer);

	switch (code)
	{
	case d_control_code(communication::e_control_code::test):
	{
		call_info->detection_status = communication::e_detection_status::clean;

		break;
	}
	case d_control_code(communication::e_control_code::initialise_protected_processes):
	{
		if ((context->protected_processes.anticheat_usermode_id == 0 && context->protected_processes.protected_process_id == 0)
			|| (context->protected_processes.protected_process_id != 0 && ntkrnl::get_eprocess(context->protected_processes.protected_process_id)) // this line is checking if cached protected process is no longer running, leaving a new context ready to be used
			)
		{
			context->protected_processes = call_info->protected_processes;

			call_info->detection_status = communication::e_detection_status::clean;
		}
		else
		{
			call_info->detection_status = communication::e_detection_status::runtime_error;
		}

		break;
	}
	case d_control_code(communication::e_control_code::is_suspicious_system_thread_present):
	{
		call_info->detection_status = system::system_thread::is_suspicious_thread_present(context);

		break;
	}
	case d_control_code(communication::e_control_code::is_suspicious_process_thread_present):
	{
		call_info->detection_status = process::process_thread::is_suspicious_thread_present(context, call_info->is_suspicious_process_thread_present);

		break;
	}
	case d_control_code(communication::e_control_code::check_reserved_msr_usage):
	{
		call_info->detection_status = hypervisor::check_reserved_msr_usage();

		break;
	}
	case d_control_code(communication::e_control_code::trigger_patchguard_bugcheck):
	{
		patchguard::trigger_bugcheck();

		call_info->detection_status = communication::e_detection_status::clean;

		break;
	}
	default:
	{
		d_log("[darken-anticheat] ioctl code is invalid, received %lu.\n", code);

		break;
	}
	}

	irp->IoStatus.Information = sizeof(communication::s_call_info);
	irp->IoStatus.Status = STATUS_SUCCESS;

	context->imports.iof_complete_request(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

void driver_unload(PDRIVER_OBJECT driver_object)
{
	context::s_context* context = context::get_decrypted();

	t_io_delete_device io_delete_device = context->imports.io_delete_device;
	t_io_delete_symbolic_link io_delete_symbolic_link = context->imports.io_delete_symbolic_link;

	context = nullptr;

	handles::permission_stripping::unload();
	context::unload();

	io_delete_device(driver_object->DeviceObject);
	io_delete_symbolic_link(&driver_info::device_symbolic_name);
}

NTSTATUS driver_entry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(registry_path);

	if (context::load() == false)
	{
		return STATUS_ABANDONED;
	}

	context::s_context* context = context::get_decrypted();

	if (imports::load(context) == false)
	{
		return STATUS_ABANDONED;
	}

	if (offsets::load(context) == false)
	{
		d_log("[darken-anticheat] failed to calculate offsets, are we on an unsupported Windows version?\n");

		return STATUS_ABANDONED;
	}

	if (handles::permission_stripping::load(context) == false)
	{
		d_log("[darken-anticheat] failed to load process handle permission stripping.\n");

		return STATUS_ABANDONED;
	}

	driver_object->MajorFunction[IRP_MJ_CREATE] = ioctl_manage_call;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = ioctl_manage_call;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ioctl_call_processor;
	driver_object->DriverUnload = driver_unload;

	NTSTATUS sanity_status = context->imports.io_create_symbolic_link(&driver_info::device_symbolic_name, &driver_info::device_name);

	if (NT_SUCCESS(sanity_status) == false)
	{
		d_log("[darken-anticheat] unable to create symbolic link between device names.\n");

		return sanity_status;
	}

	sanity_status = context->imports.io_create_device(driver_object, 0, &driver_info::device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &driver_object->DeviceObject);

	if (NT_SUCCESS(sanity_status) == false)
	{
		d_log("[darken-anticheat] unable to create io device.\n");

		return sanity_status;
	}

	// NOTE: if you are testing on a system with patchguard disabled, uncommenting this next line WILL crash your system
	//patchguard::trigger_bugcheck(); // im not sure about this line yet, control over the idt could lead them straight to this routine

	return STATUS_SUCCESS;
}
