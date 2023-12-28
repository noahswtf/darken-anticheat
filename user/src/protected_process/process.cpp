#include "process.h"
#include "../driver/driver.h"
#include <Windows.h>
#include <string>

void process::initialise()
{
    STARTUPINFO startup_info;
    ZeroMemory(&startup_info, sizeof(STARTUPINFO));
    startup_info.cb = sizeof(STARTUPINFO);

    PROCESS_INFORMATION process_information;
    ZeroMemory(&process_information, sizeof(PROCESS_INFORMATION));

    // the reason we dont just do GetCurrentDirectory is because if we're launching the binary through visual studio, the 'path'
    // will be the /user/ folder, not where the binary actually is, this way we can get the correct path
    char current_executable_path[MAX_PATH];
    GetModuleFileNameA(nullptr, current_executable_path, MAX_PATH);

    std::string current_directory(current_executable_path);

    // erase file name
    unsigned long long file_name_start = current_directory.find_last_of('\\');
    if (file_name_start != std::string::npos) current_directory.erase(file_name_start + 1ull);

    std::string protected_process_path(current_directory + '\\' + PROTECTED_PROCESS_NAME);

    if (CreateProcessA(protected_process_path.c_str(), nullptr, nullptr, nullptr, false, CREATE_NEW_CONSOLE, nullptr, nullptr, &startup_info, &process_information))
    {
        unsigned long long protected_process_id = static_cast<unsigned long long>(process_information.dwProcessId),
            current_process_id = static_cast<unsigned long long>(GetCurrentProcessId());

        // make sure there arent handles flying about
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        driver::start_protections(driver::s_core_info(current_process_id, protected_process_id));

        process_id = protected_process_id;
    }
    else
    {
        printf("couldnt create process: %lu", GetLastError());
    }
}

void process::unload()
{
    
}
