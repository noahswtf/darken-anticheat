#include "dos_header.hpp"

bool portable_executable::dos_header_t::valid() const
{
    return this->e_magic == dos_magic;
}

portable_executable::nt_headers_t* portable_executable::dos_header_t::nt_headers()
{
    return reinterpret_cast<nt_headers_t*>(reinterpret_cast<pe_crt::uintptr_t>(this) + this->e_lfanew);
}

const portable_executable::nt_headers_t* portable_executable::dos_header_t::nt_headers() const
{
    return reinterpret_cast<const nt_headers_t*>(reinterpret_cast<pe_crt::uintptr_t>(this) + this->e_lfanew);
}