#include "detections.h"
#include "handles.h"

#define CTL_CODE_T(code) CTL_CODE(FILE_DEVICE_UNKNOWN, code, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

UNICODE_STRING device_name = RTL_CONSTANT_STRING(L"\\Device\\darken-ac"),
	device_symbolic_name = RTL_CONSTANT_STRING(L"\\DosDevices\\darken-ac");

NTSTATUS ioctl_manage_call(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	irp->IoStatus.Information = 0ull;
	irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS ioctl_call_processor(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	unsigned long code = IoGetCurrentIrpStackLocation(irp)->Parameters.DeviceIoControl.IoControlCode;

	communication::s_call_info* call_info = reinterpret_cast<communication::s_call_info*>(irp->AssociatedIrp.SystemBuffer);

	switch (code)
	{
		case CTL_CODE_T(static_cast<unsigned long>(communication::e_call_code::test)):
		{
			call_info->response = communication::e_response::clean;

			break;
		}
		case CTL_CODE_T(static_cast<unsigned long>(communication::e_call_code::start_protections)):
		{
			// make sure '/integritycheck' is in linker, otherwise we will get STATUS_ACCESS_DENIED
			// when attempting to register callbacks
			handles::permission_stripping::process_ids = call_info->core_info;
			e_error error = handles::permission_stripping::initialise();

#ifdef DEBUG
			if (error == e_error::error)
			{
				DbgPrint("[darken-ac]: failed to initialise handle stripping.");
			}
#endif

			break;
		}
		case CTL_CODE_T(static_cast<unsigned long>(communication::e_call_code::check_process_suspicious_modules)):
		{
			detections::process::find_suspicious_modules(call_info);

			break;
		}
		case CTL_CODE_T(static_cast<unsigned long>(communication::e_call_code::check_process_suspicious_threads)):
		{
			detections::process::find_suspicious_threads(call_info);

			break;
		}
		case CTL_CODE_T(static_cast<unsigned long>(communication::e_call_code::check_system_suspicious_threads)):
		{
			HANDLE thread_handle;
			if (!NT_SUCCESS(PsCreateSystemThread(&thread_handle, THREAD_ALL_ACCESS, nullptr, nullptr, nullptr, reinterpret_cast<PKSTART_ROUTINE>(detections::system::find_suspicious_threads), call_info)))
			{
#ifdef DEBUG
				DbgPrint("[darken-ac]: failed to create system thread for call [check_system_suspicious_threads].");
#endif

				call_info->response = communication::e_response::runtime_error;
				break;
			}

			void* object = nullptr;
			if (!NT_SUCCESS(ObReferenceObjectByHandle(thread_handle, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &object, nullptr)))
			{
#ifdef DEBUG
				DbgPrint("[darken-ac]: failed reference object by handle [check_system_suspicious_threads].");
#endif
				
				ZwClose(thread_handle);
				call_info->response = communication::e_response::runtime_error;
				break;
			}

			if (!NT_SUCCESS(KeWaitForSingleObject(object, Executive, KernelMode, 0, nullptr)))
			{
#ifdef DEBUG
				DbgPrint("[darken-ac]: failed to wait for thread to finish [check_system_suspicious_threads].");
#endif

				call_info->response = communication::e_response::runtime_error;
			}

			ZwClose(thread_handle);
			ObfDereferenceObject(object);

			break;
		}
		case CTL_CODE_T(static_cast<unsigned long>(communication::e_call_code::check_in_virtual_machine)):
		{
			// will add more checks for virtual machine, but dont think this should have its own call code
			detections::virtual_machine::check_msr_usage(call_info);

			break;
		}
		default:
		{
			// so we know if the codes are correct etc
#ifdef DEBUG
			DbgPrint("[darken-ac]: ioctl code is invalid, received %lu.", code);
#endif

			break;
		}
	}

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = sizeof(communication::s_call_info);

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

void driver_unload(PDRIVER_OBJECT driver_object)
{
	handles::permission_stripping::unload();

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
