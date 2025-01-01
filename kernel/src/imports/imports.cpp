#include "imports.h"
#include "../context/context.h"
#include "../utilities/ntkrnl.h"
#include <portable_executable/image.hpp>
#include <string_encryption.h>

bool imports::load(context::s_context* context)
{
	const portable_executable::image_t* ntoskrnl_image = reinterpret_cast<const portable_executable::image_t*>(context->ntoskrnl_base_address);

	context->imports.nt_build_number = reinterpret_cast<uint16_t*>(ntoskrnl_image->find_export(d_encrypt_string("NtBuildNumber")));
	context->imports.ps_loaded_module_list = reinterpret_cast<uint64_t>(ntoskrnl_image->find_export(d_encrypt_string("PsLoadedModuleList")));
	context->imports.ps_process_type = reinterpret_cast<uint64_t>(ntoskrnl_image->find_export(d_encrypt_string("PsProcessType")));
	
	// ex_allocate_pool_2 set in context::load

	context->imports.ex_free_pool_with_tag = reinterpret_cast<t_ex_free_pool_with_tag>(ntoskrnl_image->find_export(d_encrypt_string("ExFreePoolWithTag")));
	context->imports.iof_complete_request = reinterpret_cast<t_iof_complete_request>(ntoskrnl_image->find_export(d_encrypt_string("IofCompleteRequest")));
	context->imports.ob_register_callbacks = reinterpret_cast<t_ob_register_callbacks>(ntoskrnl_image->find_export(d_encrypt_string("ObRegisterCallbacks")));
	context->imports.ob_unregister_callbacks = reinterpret_cast<t_ob_unregister_callbacks>(ntoskrnl_image->find_export(d_encrypt_string("ObUnRegisterCallbacks")));
	context->imports.io_create_symbolic_link = reinterpret_cast<t_io_create_symbolic_link>(ntoskrnl_image->find_export(d_encrypt_string("IoCreateSymbolicLink")));
	context->imports.io_create_device = reinterpret_cast<t_io_create_device>(ntoskrnl_image->find_export(d_encrypt_string("IoCreateDevice")));
	context->imports.io_delete_device = reinterpret_cast<t_io_delete_device>(ntoskrnl_image->find_export(d_encrypt_string("IoDeleteDevice")));
	context->imports.io_delete_symbolic_link = reinterpret_cast<t_io_delete_symbolic_link>(ntoskrnl_image->find_export(d_encrypt_string("IoDeleteSymbolicLink")));
	context->imports.ps_lookup_thread_by_thread_id = reinterpret_cast<t_ps_lookup_thread_by_thread_id>(ntoskrnl_image->find_export(d_encrypt_string("PsLookupThreadByThreadId")));
	context->imports.ps_is_system_thread = reinterpret_cast<t_ps_is_system_thread>(ntoskrnl_image->find_export(d_encrypt_string("PsIsSystemThread")));
	context->imports.ke_stack_attach_process = reinterpret_cast<t_ke_stack_attach_process>(ntoskrnl_image->find_export(d_encrypt_string("KeStackAttachProcess")));
	context->imports.ke_unstack_detach_process = reinterpret_cast<t_ke_unstack_detach_process>(ntoskrnl_image->find_export(d_encrypt_string("KeUnstackDetachProcess")));
	context->imports.mm_get_physical_address = reinterpret_cast<t_mm_get_physical_address>(ntoskrnl_image->find_export(d_encrypt_string("MmGetPhysicalAddress")));
	context->imports.mm_get_virtual_for_physical = reinterpret_cast<t_mm_get_virtual_for_physical>(ntoskrnl_image->find_export(d_encrypt_string("MmGetVirtualForPhysical")));

	return true;
}
