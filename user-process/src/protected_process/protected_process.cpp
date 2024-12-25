#include "protected_process.h"
#include <driver/driver.h>

#include <Windows.h>
#include <string>

#define d_protected_process_file_name "darken-anticheat-protected-process.exe"

bool protected_process::load()
{
    PROCESS_INFORMATION process_information = { };
    STARTUPINFO startup_info = { };

    startup_info.cb = sizeof(STARTUPINFO);

    std::string current_executable_path(MAX_PATH, '\0');
    GetModuleFileNameA(nullptr, current_executable_path.data(), MAX_PATH);

    std::string current_directory = current_executable_path.substr(0, current_executable_path.find_last_of('\\'));

    std::string protected_process_path = current_directory + '\\' + d_protected_process_file_name;

    if (CreateProcessA(protected_process_path.c_str(), nullptr, nullptr, nullptr, false, CREATE_NEW_CONSOLE, nullptr, nullptr, &startup_info, &process_information))
    {
        CloseHandle(process_information.hProcess);
        CloseHandle(process_information.hThread);

        communication::s_call_info call_info = { };

        call_info.protected_processes.anticheat_usermode_id = static_cast<std::uint64_t>(GetCurrentProcessId());
        call_info.protected_processes.protected_process_id = static_cast<std::uint64_t>(process_information.dwProcessId);

        process_id = call_info.protected_processes.protected_process_id;

        return driver::send_call(communication::e_control_code::initialise_protected_processes, call_info).detection_status == communication::e_detection_status::clean;
    }

    return false;
}
