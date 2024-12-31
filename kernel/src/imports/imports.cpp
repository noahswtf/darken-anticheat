#include "imports.h"
#include "../context/context.h"
#include "../utilities/ntkrnl.h"
#include <portable_executable/image.hpp>
#include <string_encryption.h>

#include "../log.h"
#include <ntifs.h>

bool imports::load()
{
	context::s_context* context = context::get_decrypted();

	const portable_executable::image_t* ntoskrnl_image = reinterpret_cast<const portable_executable::image_t*>(context->ntoskrnl_base_address);

	context->imports.nt_build_number = reinterpret_cast<uint16_t*>(ntoskrnl_image->find_export(d_encrypt_string("NtBuildNumber")));
	context->imports.iof_complete_request = reinterpret_cast<t_iof_complete_request>(ntoskrnl_image->find_export(d_encrypt_string("IofCompleteRequest")));
	context->imports.ob_register_callbacks = reinterpret_cast<t_ob_register_callbacks>(ntoskrnl_image->find_export(d_encrypt_string("ObRegisterCallbacks")));

	return true;
}
