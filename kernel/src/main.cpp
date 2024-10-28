#include "handles/permission_stripping.h"
#include "system/system_thread.h"
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

	IofCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS ioctl_call_processor(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	unsigned long code = IoGetCurrentIrpStackLocation(irp)->Parameters.DeviceIoControl.IoControlCode;

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
		if (handles::permission_stripping::protected_processes.anticheat_usermode_id == 0 && handles::permission_stripping::protected_processes.protected_process_id == 0)
		{
			handles::permission_stripping::protected_processes = call_info->protected_processes;

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
		call_info->detection_status = system::system_thread::is_suspicious_thread_present();

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

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

void driver_unload(PDRIVER_OBJECT driver_object)
{
	handles::permission_stripping::unload();

	IoDeleteDevice(driver_object->DeviceObject);
	IoDeleteSymbolicLink(&driver_info::device_symbolic_name);
}

NTSTATUS driver_entry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(registry_path);

	if (offsets::load() == false)
	{
		d_log("[darken-anticheat] failed to calculate offsets, are we on an unsupported Windows version?\n");

		return STATUS_ABANDONED;
	}

	if (handles::permission_stripping::load() == false)
	{
		d_log("[darken-anticheat] failed to load process handle permission stripping.\n");

		return STATUS_ABANDONED;
	}

	IoCreateSymbolicLink(&driver_info::device_symbolic_name, &driver_info::device_name);

	NTSTATUS sanity_status = IoCreateDevice(driver_object, 0, &driver_info::device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &driver_object->DeviceObject);

	if (NT_SUCCESS(sanity_status) == false)
	{
		d_log("[darken-anticheat] unable to create io device.\n");

		return sanity_status;
	}

	driver_object->MajorFunction[IRP_MJ_CREATE] = ioctl_manage_call;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = ioctl_manage_call;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ioctl_call_processor;
	driver_object->DriverUnload = driver_unload;

	return STATUS_SUCCESS;
}
