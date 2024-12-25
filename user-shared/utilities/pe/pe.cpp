#include "pe.h"
#include <imports.h>
#include "../http/http.h"
#include "../pdb/pdb.h"

#include <Windows.h>
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>

bool utilities::pe::is_digitally_signed(std::wstring_view binary_path)
{
	WINTRUST_FILE_INFO file_info = { };

	file_info.cbStruct = sizeof(WINTRUST_FILE_INFO);
	file_info.pcwszFilePath = binary_path.data();
	file_info.hFile = nullptr;
	file_info.pgKnownSubject = nullptr;

	GUID wvt_policy_guid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA win_trust_data = { };

	win_trust_data.cbStruct = sizeof(win_trust_data);
	win_trust_data.pPolicyCallbackData = nullptr;
	win_trust_data.pSIPClientData = nullptr;
	win_trust_data.dwUIChoice = WTD_UI_NONE;
	win_trust_data.fdwRevocationChecks = WTD_REVOKE_WHOLECHAIN;
	win_trust_data.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL;
	win_trust_data.dwUnionChoice = WTD_CHOICE_FILE;
	win_trust_data.dwStateAction = WTD_STATEACTION_VERIFY;
	win_trust_data.hWVTStateData = nullptr;
	win_trust_data.pwszURLReference = nullptr;
	win_trust_data.dwUIContext = 0;
	win_trust_data.pFile = &file_info;

	bool is_digitally_signed = d_import(WinVerifyTrust)(nullptr, &wvt_policy_guid, &win_trust_data) == ERROR_SUCCESS;

	win_trust_data.dwStateAction = WTD_STATEACTION_CLOSE;

	d_import(WinVerifyTrust)(nullptr, &wvt_policy_guid, &win_trust_data);

	return is_digitally_signed;
}

bool utilities::pe::has_pdb_on_microsoft_symbol_server(std::wstring_view binary_path)
{
	s_pdb_info pdb_info = pdb::get_pdb_info(binary_path);

	if (pdb_info.guid.empty() == true || pdb_info.pdb_name.empty() == true || pdb_info.age.empty() == true)
	{
		return false;
	}

	return http::does_url_exist("msdl.microsoft.com", "/download/symbols/" + pdb_info.pdb_name + "/" + pdb_info.guid + pdb_info.age + "/" + pdb_info.pdb_name);
}
