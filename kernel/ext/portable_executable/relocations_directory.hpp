#pragma once

#include "pe_crt/crt_type_traits.hpp"
#include "pe_crt/crt_types.hpp"

namespace portable_executable
{
    enum class relocation_type_t : pe_crt::uint16_t
    {
        absolute,
        high,
        low,
        high_low,
        high_adj,
        machine_specific_5,
        reserved,
        machine_specific_7,
        machine_specific_8,
        machine_specific_9,
        dir64
    };

    struct relocation_entry_descriptor_t
    {
        pe_crt::uint16_t offset : 12;
        relocation_type_t type : 4;
    };

    struct raw_relocation_block_descriptor_t
    {
        pe_crt::uint32_t virtual_address;
        pe_crt::uint32_t size_of_block;
    };

    struct relocation_block_t
    {
        pe_crt::uint32_t current_entry_index;
        pe_crt::uint32_t max_entry_index;
    };

    struct relocation_entry_t
    {
        relocation_entry_descriptor_t descriptor;
        pe_crt::uint32_t virtual_address;
    };

    class relocations_iterator_t
    {
    private:
        const raw_relocation_block_descriptor_t* current_raw_relocation_block_descriptor = nullptr;
        relocation_block_t current_relocation_block = { };

        const relocation_entry_descriptor_t* current_descriptor = nullptr;

        void load_block(const raw_relocation_block_descriptor_t* raw_relocation_block_descriptor);

    public:
        relocations_iterator_t() = default;

        relocations_iterator_t(const raw_relocation_block_descriptor_t* raw_relocation_block_descriptor);

        using value_type = relocation_entry_t;
        using pointer = value_type*;
        using reference = value_type&;

        value_type operator*() const;

        relocations_iterator_t& operator++();

        bool operator==(const relocations_iterator_t& other);

        bool operator!=(const relocations_iterator_t& other);
    };

    template<typename T>
    class relocations_range_t
    {
    private:
        using pointer_type = typename pe_crt::conditional<pe_crt::is_const<T>::value, const pe_crt::uint8_t*, pe_crt::uint8_t*>::type;
        using relocation_descriptor_type = typename pe_crt::conditional<pe_crt::is_const<T>::value, const raw_relocation_block_descriptor_t*, raw_relocation_block_descriptor_t*>::type;

        pointer_type m_module = nullptr;

        const raw_relocation_block_descriptor_t* m_raw_relocation_block_descriptor = nullptr;

    public:
        relocations_range_t() = default;

        relocations_range_t(pointer_type module, pe_crt::uint32_t relocations_rva) :
            m_module(module), m_raw_relocation_block_descriptor(reinterpret_cast<relocation_descriptor_type>(module + relocations_rva))
        {

        }

        T begin() const
        {
            return { this->m_raw_relocation_block_descriptor };
        }

        T end() const
        {
            return { nullptr };
        }
    };
}