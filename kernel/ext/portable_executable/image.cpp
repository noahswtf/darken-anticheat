#include "image.hpp"

#include "pe_crt/crt_string.hpp"

portable_executable::dos_header_t* portable_executable::image_t::dos_header()
{
	return &this->m_dos_header;
}

const portable_executable::dos_header_t* portable_executable::image_t::dos_header() const
{
	return &this->m_dos_header;
}

portable_executable::nt_headers_t* portable_executable::image_t::nt_headers()
{
	return this->dos_header()->nt_headers();
}

const portable_executable::nt_headers_t* portable_executable::image_t::nt_headers() const
{
	return this->dos_header()->nt_headers();
}

portable_executable::section_header_t* portable_executable::image_t::find_section(const char* name)
{
	for (auto& section : this->sections())
	{
		if (pe_crt::strstr(section.name, name))
		{
			return &section;
		}
	}

	return nullptr;
}

const portable_executable::section_header_t* portable_executable::image_t::find_section(const char* name) const
{
	for (const auto& section : this->sections())
	{
		if (pe_crt::strstr(section.name, name))
		{
			return &section;
		}
	}

	return nullptr;
}

portable_executable::pe_crt::uint8_t* portable_executable::image_t::find_export(const char* name) const
{
	for (const auto& [export_name, export_address] : this->exports())
	{
		if (pe_crt::strstr(export_name, name))
		{
			return export_address;
		}
	}

	return nullptr;
}

portable_executable::pe_crt::uint8_t* portable_executable::image_t::signature_scan(const char* signature) const
{
	constexpr auto in_range = [=](char x, char a, char b) -> bool
	{
		return x >= a && x <= b;
	};

	constexpr auto get_bits = [=](char x) -> pe_crt::int8_t
	{
		if (x >= 'a' && x <= 'f')
		{
			x -= 0x20;
		}

		if (in_range(x, 'A', 'F'))
		{
			return x - 'A' + 0xA;
		}
		else if (in_range(x, '0', '9'))
		{
			return x - '0';
		}

		return 0;
	};

	constexpr auto get_byte = [=](const char* x) -> pe_crt::uint8_t
	{
		return static_cast<pe_crt::uint8_t>((get_bits(x[0]) << 4) | get_bits(x[1]));
	};

	constexpr auto compute_pattern_size = [](const char* pattern) -> pe_crt::size_t
	{
		pe_crt::size_t pattern_size = 0;

		while (*pattern)
		{
			if (*pattern == ' ')
			{
				pattern++;

				continue;
			}

			if (*pattern == '?')
			{
				pattern_size++;
				pattern++;

				continue;
			}

			if (((*pattern >= '0' && *pattern <= '9') ||
				(*pattern >= 'A' && *pattern <= 'F') ||
				(*pattern >= 'a' && *pattern <= 'f')) && 
				((*(pattern + 1) >= '0' && *(pattern + 1) <= '9') ||
				(*(pattern + 1) >= 'A' && *(pattern + 1) <= 'F')
				|| (*(pattern + 1) >= 'a' && *(pattern + 1) <= 'f')))
			{
				pattern_size++;
				pattern += 2;

				continue;
			}

			pattern++;
		}

		return pattern_size;
	};

	pe_crt::size_t pattern_size = compute_pattern_size(signature);

	if (!pattern_size)
	{
		return nullptr;
	}

	for (const auto& section : this->sections())
	{
		const pe_crt::uint8_t* section_start = this->as<const pe_crt::uint8_t*>() + section.virtual_address;
		const pe_crt::uint8_t* section_end = section_start + section.virtual_size;

		for (const pe_crt::uint8_t* byte = section_start; byte < (section_end - pattern_size); byte++)
		{
			const char* temp_signature = signature;

			bool found = true;

			for (pe_crt::size_t j = 0; j < pattern_size; j++)
			{
				if (*temp_signature != '?' && get_byte(temp_signature) != byte[j])
				{
					found = false;
					break;
				}

				temp_signature += *temp_signature != '\?' ? 3 : 2;
			}

			if (found)
			{
				return const_cast<pe_crt::uint8_t*>(byte);
			}
		}
	}

	return nullptr;
}

portable_executable::pe_crt::uint8_t* portable_executable::image_t::signature_scan(const pe_crt::uint8_t* pattern, pe_crt::size_t pattern_size) const
{
	for (const auto& section : this->sections())
	{
		const pe_crt::uint8_t* section_start = this->as<const pe_crt::uint8_t*>() + section.virtual_address;
		const pe_crt::uint8_t* section_end = section_start + section.virtual_size;

		for (const pe_crt::uint8_t* byte = section_start; byte < (section_end - pattern_size); byte++)
		{
			bool found = true;

			for (pe_crt::size_t j = 0; j < pattern_size; j++)
			{
				if (pattern[j] != 0x00 && pattern[j] != byte[j])
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return const_cast<pe_crt::uint8_t*>(byte);
			}
		}
	}

	return nullptr;
}
