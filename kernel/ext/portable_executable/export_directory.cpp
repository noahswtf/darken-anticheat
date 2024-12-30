#include "export_directory.hpp"

#include "pe_crt/crt_string.hpp"

portable_executable::exports_iterator_t::exports_iterator_t(const pe_crt::uint8_t* module, const pe_crt::uint32_t* names, const pe_crt::uint32_t* functions, const pe_crt::uint16_t* ordinals, pe_crt::uint32_t index) :
	module(module), names(names), functions(functions), ordinals(ordinals), index(index)
{

}

portable_executable::exports_iterator_t::value_type portable_executable::exports_iterator_t::operator*() const
{
	pe_crt::uint32_t name_offset = this->names[this->index];

	pe_crt::uint16_t ordinal_offset = this->ordinals[this->index];

	pe_crt::uint32_t functions_offset = this->functions[ordinal_offset];

	auto export_name = reinterpret_cast<const char*>(this->module + name_offset);

	value_type type;

	pe_crt::strcpy(type.name, export_name);
	type.address = const_cast<pe_crt::uint8_t*>(this->module + functions_offset);

	return type;
}

portable_executable::exports_iterator_t& portable_executable::exports_iterator_t::operator++()
{
	++this->index;

	return *this;
}

bool portable_executable::exports_iterator_t::operator==(const exports_iterator_t& other)
{
	return this->index == other.index;
}

bool portable_executable::exports_iterator_t::operator!=(const exports_iterator_t& other)
{
	return this->index != other.index;
}
