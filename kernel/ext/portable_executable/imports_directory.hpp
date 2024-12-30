#pragma once

#include "pe_crt/crt_type_traits.hpp"
#include "pe_crt/crt_types.hpp"

namespace portable_executable
{
    struct import_descriptor_t
    {
        union
        {
            pe_crt::uint32_t characteristics;
            pe_crt::uint32_t original_first_thunk;
        } misc;

        pe_crt::uint32_t time_date_stamp;
        pe_crt::uint32_t forwarder_chain;
        pe_crt::uint32_t name;
        pe_crt::uint32_t first_thunk;
    };

#pragma warning(push)
#pragma warning(disable: 4201)

    struct thunk_data_t
    {
        union
        {
            pe_crt::uint64_t forwarder_string;
            pe_crt::uint64_t function;
            pe_crt::uint64_t address;

            struct
            {
                pe_crt::uint64_t ordinal : 16;
                pe_crt::uint64_t _reserved0 : 47;
                pe_crt::uint64_t is_ordinal : 1;
            };
        };
    };

#pragma warning(pop)

    struct import_by_name_t
    {
        pe_crt::uint16_t hint;
        char name[1];
    };

    struct import_entry_t
    {
        char module_name[256];
        char import_name[256];

        pe_crt::uint8_t* address;
    };

    class imports_iterator_t
    {
    private:
        const pe_crt::uint8_t* module = nullptr;

        const import_descriptor_t* current_descriptor = nullptr;
        const thunk_data_t* current_thunk = nullptr;
        const thunk_data_t* original_thunk = nullptr;

    public:
        imports_iterator_t(const pe_crt::uint8_t* module, const import_descriptor_t* descriptor);

        using value_type = import_entry_t;
        using pointer = value_type*;
        using reference = value_type&;

        value_type operator*() const;

        imports_iterator_t& operator++();

        bool operator==(const imports_iterator_t& other);

        bool operator!=(const imports_iterator_t& other);
    };
    
    template<typename T>
    class imports_range_t
    {
    private:
        using pointer_type = typename pe_crt::conditional<pe_crt::is_const<T>::value, const pe_crt::uint8_t*, pe_crt::uint8_t*>::type;
        using import_descriptor_type = typename pe_crt::conditional<pe_crt::is_const<T>::value, const import_descriptor_t*, import_descriptor_t*>::type;

        pointer_type m_module = nullptr;

        import_descriptor_type m_import_descriptor = nullptr;

    public:
        imports_range_t() = default;

        imports_range_t(pointer_type module, pe_crt::uint32_t imports_rva) :
            m_module(module), m_import_descriptor(reinterpret_cast<import_descriptor_type>(module + imports_rva))
        {

        }

        T begin() const
        {
            return { this->m_module, this->m_import_descriptor };
        }

        T end() const
        {
            return { this->m_module, nullptr };
        }
    };
}