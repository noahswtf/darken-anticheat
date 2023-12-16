#include "utilities.h"
#include <Windows.h>
#include <Psapi.h>

wchar_t* utilities::get_base_module_name()
{
    wchar_t buffer[MAX_PATH];

    HMODULE base_module = GetModuleHandle(NULL);
    HANDLE process = GetCurrentProcess();

    K32GetModuleBaseNameW(process, base_module, buffer, MAX_PATH);

    return buffer;
}
