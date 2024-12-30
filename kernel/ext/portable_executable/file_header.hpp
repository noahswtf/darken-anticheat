#pragma once

#include "pe_crt/crt_types.hpp"

namespace portable_executable
{
    struct file_header_t
    {
        pe_crt::uint16_t machine;
        pe_crt::uint16_t number_of_sections;
        pe_crt::uint32_t time_date_stamp;
        pe_crt::uint32_t pointer_to_symbol_table;
        pe_crt::uint32_t number_of_symbols;
        pe_crt::uint16_t sizeof_optional_header;
        pe_crt::uint16_t characteristics;
    };
}
