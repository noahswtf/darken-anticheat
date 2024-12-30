#include "nt_headers.hpp"

bool portable_executable::nt_headers_t::valid() const
{
	return this->signature == nt_magic;
}

portable_executable::section_header_t* portable_executable::nt_headers_t::section_headers()
{
	return reinterpret_cast<section_header_t*>(reinterpret_cast<portable_executable::pe_crt::uintptr_t>(&this->optional_header) + this->file_header.sizeof_optional_header);
}

const portable_executable::section_header_t* portable_executable::nt_headers_t::section_headers() const
{
	return reinterpret_cast<const section_header_t*>(reinterpret_cast<portable_executable::pe_crt::uintptr_t>(&this->optional_header) + this->file_header.sizeof_optional_header);
}

portable_executable::pe_crt::uint16_t portable_executable::nt_headers_t::num_sections() const
{
	return this->file_header.number_of_sections;
}