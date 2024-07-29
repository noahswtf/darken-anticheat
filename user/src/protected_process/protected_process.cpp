#include "protected_process.h"
#include "../driver/driver.h"
#include "../log/log.h"
#include <Windows.h>
#include <string>

bool protected_process::load()
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

    if (file_name_start != std::string::npos)
    {
        current_directory.erase(file_name_start + 1ull);
    }

    std::string protected_process_path(current_directory + '\\' + PROTECTED_PROCESS_NAME);

    if (CreateProcessA(protected_process_path.c_str(), nullptr, nullptr, nullptr, false, CREATE_NEW_CONSOLE, nullptr, nullptr, &startup_info, &process_information))
    {
        // make sure there arent handles flying about
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        driver::s_protected_processes protected_processes = { };
        protected_processes.user_mode_id = driver::host_process_id;
        protected_processes.protected_process_id = static_cast<unsigned long long>(process_information.dwProcessId);

        driver::initialise_protected_processes(protected_processes);

        process_id = protected_processes.protected_process_id;
    }
    else
    {
        return false;
    }

    return true;
}

void protected_process::unload()
{
    
}
