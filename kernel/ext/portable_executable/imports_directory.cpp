#include "imports_directory.hpp"

#include "pe_crt/crt_string.hpp"

portable_executable::imports_iterator_t::imports_iterator_t(const pe_crt::uint8_t* module, const import_descriptor_t* descriptor) :
	module(module), current_descriptor(descriptor)
{
    if (this->current_descriptor && this->current_descriptor->first_thunk)
    {
        this->current_thunk = reinterpret_cast<const thunk_data_t*>(this->module + current_descriptor->first_thunk);
        this->original_thunk = reinterpret_cast<const thunk_data_t*>(this->module + current_descriptor->misc.original_first_thunk);
    }
}

portable_executable::imports_iterator_t::value_type portable_executable::imports_iterator_t::operator*() const
{
    value_type value;

    if (this->original_thunk->is_ordinal)
    {
        auto import_name = reinterpret_cast<const char*>(this->module + this->original_thunk->ordinal);

        pe_crt::strcpy(value.module_name, import_name);
    }
    else
    {
        auto import_by_name = reinterpret_cast<const import_by_name_t*>(this->module + this->original_thunk->address);

        pe_crt::strcpy(value.module_name, import_by_name->name);
    }
    
    auto module_name = reinterpret_cast<const char*>(this->module + this->current_descriptor->name);

    pe_crt::strcpy(value.module_name, module_name);

    value.address = reinterpret_cast<pe_crt::uint8_t*>(this->current_thunk->function);

    return value;
}

portable_executable::imports_iterator_t& portable_executable::imports_iterator_t::operator++()
{
    if (this->current_thunk && this->current_thunk->address)
    {
        ++this->current_thunk;
        ++this->original_thunk;

        if (!this->current_thunk->address)
        {
            ++this->current_descriptor;

            while (this->current_descriptor && this->current_descriptor->first_thunk)
            {
                this->current_thunk = reinterpret_cast<const thunk_data_t*>(this->module + this->current_descriptor->first_thunk);
                this->original_thunk = reinterpret_cast<const thunk_data_t*>(this->module + this->current_descriptor->misc.original_first_thunk);

                if (this->current_thunk->address)
                {
                    break;
                }

                ++this->current_descriptor;
            }

            if (!this->current_descriptor || !this->current_descriptor->first_thunk)
            {
                this->current_descriptor = nullptr;
                this->current_thunk = nullptr;
            }
        }
    }

    return *this;
}

bool portable_executable::imports_iterator_t::operator==(const imports_iterator_t& other)
{
    return this->current_descriptor == other.current_descriptor && this->current_thunk == other.current_thunk;
}

bool portable_executable::imports_iterator_t::operator!=(const imports_iterator_t& other)
{
    return this->current_descriptor != other.current_descriptor || this->current_thunk != other.current_thunk;
}
