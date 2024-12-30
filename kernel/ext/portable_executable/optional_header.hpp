#pragma once

#include "pe_crt/crt_types.hpp"

#include "data_directory.hpp"

namespace portable_executable
{
    struct optional_header_t
    {
        pe_crt::uint16_t magic;
        pe_crt::uint8_t major_linker_version;
        pe_crt::uint8_t minor_linker_version;
        pe_crt::uint32_t size_of_code;
        pe_crt::uint32_t size_of_initialized_data;
        pe_crt::uint32_t size_of_uninitialized_data;
        pe_crt::uint32_t address_of_entry_point;
        pe_crt::uint32_t base_of_code;
        pe_crt::uint64_t image_base;
        pe_crt::uint32_t section_alignment;
        pe_crt::uint32_t file_alignment;
        pe_crt::uint16_t major_operating_system_version;
        pe_crt::uint16_t minor_operating_system_version;
        pe_crt::uint16_t major_image_version;
        pe_crt::uint16_t minor_image_version;
        pe_crt::uint16_t major_subsystem_version;
        pe_crt::uint16_t minor_subsystem_version;
        pe_crt::uint32_t win32_version_value;
        pe_crt::uint32_t size_of_image;
        pe_crt::uint32_t size_of_headers;
        pe_crt::uint32_t check_sum;
        pe_crt::uint16_t subsystem;
        pe_crt::uint16_t dll_characteristics;
        pe_crt::uint64_t size_of_stack_reserve;
        pe_crt::uint64_t size_of_stack_commit;
        pe_crt::uint64_t size_of_heap_reserve;
        pe_crt::uint64_t size_of_heap_commit;
        pe_crt::uint32_t loader_flags;
        pe_crt::uint32_t number_of_rva_and_sizes;
        data_directories_t data_directories;
    };
}