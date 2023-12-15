#include <ntifs.h>
#include <ntddk.h>

#define CTL_CODE_T(code) CTL_CODE(FILE_DEVICE_UNKNOWN, code, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_CODE_TEST CTL_CODE_T(0x1500)

UNICODE_STRING device_name = RTL_CONSTANT_STRING(L"\\Device\\darken-ac"),
	device_symbolic_name = RTL_CONSTANT_STRING(L"\\DosDevices\\darken-ac");

NTSTATUS ioctl_manage_call(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	irp->IoStatus.Information = 0ULL;
	irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

enum class e_response : unsigned long long
{
	flagged,
	clean
};

struct s_call_info
{
	unsigned long control_code = 0ul;

	e_response response = e_response::flagged;
};

NTSTATUS ioctl_call_processor(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	unsigned long code = IoGetCurrentIrpStackLocation(irp)->Parameters.DeviceIoControl.IoControlCode;

	switch (code)
	{
	case IO_CODE_TEST:
	{
		s_call_info* call_info = reinterpret_cast<s_call_info*>(irp->AssociatedIrp.SystemBuffer);

		call_info->response = e_response::clean;

		*reinterpret_cast<s_call_info**>(irp->AssociatedIrp.SystemBuffer) = call_info;

#ifdef _DEBUG
		DbgPrint("[darken-ac]: running IO_CODE_TEMPLATE respective call.");
#endif

		break;
	}
	default:
	{
		// so we know if the codes are correct etc
#ifdef _DEBUG
		DbgPrint("[darken-ac]: ioctl code is invalid.");
#endif

		break;
	}
	}

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = sizeof(s_call_info);

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


void driver_unload(PDRIVER_OBJECT driver_object)
{
	IoDeleteDevice(driver_object->DeviceObject);
	IoDeleteSymbolicLink(&device_symbolic_name);
}

NTSTATUS driver_entry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);

	IoCreateSymbolicLink(&device_symbolic_name, &device_name);

	NTSTATUS sanity_status = IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &driver_object->DeviceObject);

	if (!NT_SUCCESS(sanity_status))
	{
#ifdef DEBUG
		DbgPrint("[darken-ac]: unable to create io device.");
#endif

		return sanity_status;
	}

	driver_object->MajorFunction[IRP_MJ_CREATE] = driver_object->MajorFunction[IRP_MJ_CLOSE] = ioctl_manage_call;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ioctl_call_processor;
	driver_object->DriverUnload = driver_unload;

	return sanity_status;
}
