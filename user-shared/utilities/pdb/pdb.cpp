#include "pdb.h"
#include "../datatype/datatype.h"
#include "../filesystem/filesystem.h"

#include <Windows.h>

s_pdb_info utilities::pdb::get_pdb_info(std::wstring_view binary_path)
{
    std::vector<std::uint8_t> raw_binary = filesystem::read_file(binary_path);

    if (raw_binary.empty() == true)
    {
        return { };
    }

    PIMAGE_DOS_HEADER binary_dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(raw_binary.data());

    if (binary_dos_header->e_magic != 0x5a4d)
    {
        return { };
    }

    PIMAGE_NT_HEADERS binary_nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(raw_binary.data() + binary_dos_header->e_lfanew);

    auto debug_directory_offset = binary_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;

    if (debug_directory_offset == 0)
    {
        return { };
    }

    PIMAGE_SECTION_HEADER current_section = IMAGE_FIRST_SECTION(binary_nt_headers);
    PIMAGE_DEBUG_DIRECTORY debug_directory = nullptr;

    for (std::uint16_t i = 0; i < binary_nt_headers->FileHeader.NumberOfSections; i++, current_section++)
    {
        if (current_section->VirtualAddress <= debug_directory_offset && debug_directory_offset < current_section->VirtualAddress + current_section->Misc.VirtualSize)
        {
            debug_directory = reinterpret_cast<PIMAGE_DEBUG_DIRECTORY>(raw_binary.data() + current_section->PointerToRawData + (debug_directory_offset - current_section->VirtualAddress));

            break;
        }
    }

    if (debug_directory == nullptr)
    {
        return { };
    }

    std::uint8_t* binary_debug_directory_raw_data = raw_binary.data() + debug_directory->PointerToRawData + 4;

    s_pdb_info pdb_info = { };
    GUID raw_guid = { };

    memcpy(&raw_guid, binary_debug_directory_raw_data, sizeof(GUID));
    pdb_info.guid = datatype::guid::to_string(raw_guid);

    std::uint32_t raw_age = 0;

    memcpy(&raw_age, binary_debug_directory_raw_data + sizeof(GUID), sizeof(std::uint32_t));
    pdb_info.age = datatype::uint32::to_hexadecimal_string(raw_age);

    pdb_info.pdb_name = reinterpret_cast<const char*>(binary_debug_directory_raw_data + sizeof(GUID) + sizeof(std::uint32_t));

    return pdb_info;
}

