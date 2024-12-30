#pragma once

#include "pe_crt/crt_type_traits.hpp"
#include "pe_crt/crt_types.hpp"
#include "pe_crt/crt_memory.hpp"

namespace portable_executable
{
    struct export_entry_t
    {
        char name[256];

        pe_crt::uint8_t* address;

        export_entry_t() : address(nullptr) { pe_crt::memset(this->name, 0, sizeof this->name); }
    };

    class exports_iterator_t
    {
    private:
        const pe_crt::uint8_t* module = nullptr;
        const pe_crt::uint32_t* names = nullptr;
        const pe_crt::uint32_t* functions = nullptr;
        const pe_crt::uint16_t* ordinals = nullptr;
        pe_crt::uint32_t index = 0;

    public:
        exports_iterator_t(const pe_crt::uint8_t* module, const pe_crt::uint32_t* names, const pe_crt::uint32_t* functions, const pe_crt::uint16_t* ordinals, pe_crt::uint32_t index);

        using value_type = export_entry_t;
        using pointer = value_type*;
        using reference = value_type&;

        value_type operator*() const;

        exports_iterator_t& operator++();

        bool operator==(const exports_iterator_t& other);

        bool operator!=(const exports_iterator_t& other);
    };

    template<typename T>
    class exports_range_t
    {
    private:
        using pointer_type = typename pe_crt::conditional<pe_crt::is_const<T>::value, const pe_crt::uint8_t*, pe_crt::uint8_t*>::type;
        using uint32_pointer = typename pe_crt::conditional<pe_crt::is_const<T>::value, const pe_crt::uint32_t*, pe_crt::uint32_t*>::type;
        using uint16_pointer = typename pe_crt::conditional<pe_crt::is_const<T>::value, const pe_crt::uint16_t*, pe_crt::uint16_t*>::type;

        pointer_type m_module = nullptr;
        uint32_pointer m_names = nullptr;
        uint32_pointer m_functions = nullptr;
        uint16_pointer m_ordinals = nullptr;

        pe_crt::uint32_t m_num_exports = 0;

    public:
        exports_range_t() = default;

        exports_range_t(pointer_type module, uint32_pointer names, uint32_pointer functions, uint16_pointer ordinals, pe_crt::uint32_t num_exports) :
            m_module(module), m_names(names), m_functions(functions), m_ordinals(ordinals), m_num_exports(num_exports)
        {

        }

        T begin() const
        {
            return { this->m_module, this->m_names, this->m_functions, this->m_ordinals, 0 };
        }

        T end() const
        {
            return { this->m_module, this->m_names, this->m_functions, this->m_ordinals, this->m_num_exports };
        }
    };

    struct export_directory_t
    {
        pe_crt::uint32_t characteristics;
        pe_crt::uint32_t time_date_stamp;
        pe_crt::uint16_t major_version;
        pe_crt::uint16_t minor_version;
        pe_crt::uint32_t name;
        pe_crt::uint32_t base;
        pe_crt::uint32_t number_of_functions;
        pe_crt::uint32_t number_of_names;
        pe_crt::uint32_t address_of_functions;
        pe_crt::uint32_t address_of_names;
        pe_crt::uint32_t address_of_name_ordinals;
    };
}