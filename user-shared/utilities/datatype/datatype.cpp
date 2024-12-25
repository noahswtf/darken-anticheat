#include "datatype.h"
#include <iomanip>
#include <sstream>

std::string utilities::datatype::guid::to_string(GUID guid)
{
    std::ostringstream string_stream = { };

    string_stream << std::hex << std::uppercase << std::setfill('0');
    string_stream << std::setw(8) << guid.Data1;
    string_stream << std::setw(4) << guid.Data2;
    string_stream << std::setw(4) << guid.Data3;

    for (int i = 0; i < 8; i++)
    {
        string_stream << std::setw(2) << static_cast<int>(guid.Data4[i]);
    }

    return string_stream.str();
}

std::string utilities::datatype::uint32::to_hexadecimal_string(std::uint32_t integral_number)
{
    std::ostringstream string_stream = { };

    string_stream << std::setw(1) << std::hex << integral_number;

    return string_stream.str();
}