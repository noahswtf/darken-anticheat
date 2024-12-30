#include "imports.h"
#include "../context/context.h"
#include "../utilities/ntkrnl.h"
#include <portable_executable/image.hpp>
#include <string_encryption.h>

bool imports::load()
{
	context::s_context* context = context::get_decrypted();

	uint64_t ntoskrnl_base_address = ntkrnl::get_process_base_address(context->initial_system_process);

	if (ntoskrnl_base_address == 0)
	{
		return false;
	}

	const portable_executable::image_t* ntoskrnl_image = reinterpret_cast<const portable_executable::image_t*>(ntoskrnl_base_address);

	context->imports.nt_build_number = reinterpret_cast<uint16_t*>(ntoskrnl_image->find_export(d_encrypt_string("NtBuildNumber")));
	context->imports.iof_complete_request = reinterpret_cast<t_iof_complete_request>(ntoskrnl_image->find_export(d_encrypt_string("IofCompleteRequest")));

	return true;
}
