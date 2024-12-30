#pragma once

#include "pe_crt/crt_types.hpp"

namespace portable_executable
{
    static constexpr pe_crt::size_t section_name_size_limit = 8;

#pragma warning(push)
#pragma warning(disable: 4201)

    union section_characteristics_t
    {
        struct
        {
            pe_crt::uint32_t _pad0 : 5;
            pe_crt::uint32_t cnt_code : 1;
            pe_crt::uint32_t cnt_init_data : 1;
            pe_crt::uint32_t cnt_uninit_data : 1;
            pe_crt::uint32_t _pad1 : 1;
            pe_crt::uint32_t lnk_info : 1;
            pe_crt::uint32_t _pad2 : 1;
            pe_crt::uint32_t lnk_remove : 1;
            pe_crt::uint32_t lnk_comdat : 1;
            pe_crt::uint32_t _pad3 : 1;
            pe_crt::uint32_t no_defer_spec_exc : 1;
            pe_crt::uint32_t mem_far : 1;
            pe_crt::uint32_t _pad4 : 1;
            pe_crt::uint32_t mem_purgeable : 1;
            pe_crt::uint32_t mem_locked : 1;
            pe_crt::uint32_t mem_preload : 1;
            pe_crt::uint32_t alignment : 4;
            pe_crt::uint32_t lnk_nreloc_ovfl : 1;
            pe_crt::uint32_t mem_discardable : 1;
            pe_crt::uint32_t mem_not_cached : 1;
            pe_crt::uint32_t mem_not_paged : 1;
            pe_crt::uint32_t mem_shared : 1;
            pe_crt::uint32_t mem_execute : 1;
            pe_crt::uint32_t mem_read : 1;
            pe_crt::uint32_t mem_write : 1;
        };

        pe_crt::uint32_t flags;
    };

#pragma warning(pop)

    struct section_header_t
    {
        char name[section_name_size_limit];
        pe_crt::uint32_t virtual_size;
        pe_crt::uint32_t virtual_address;
        pe_crt::uint32_t size_of_raw_data;
        pe_crt::uint32_t pointer_to_raw_data;
        pe_crt::uint32_t pointer_to_relocations;
        pe_crt::uint32_t pointer_to_linenumbers;
        pe_crt::uint16_t number_of_relocations;
        pe_crt::uint16_t number_of_linenumbers;
        section_characteristics_t characteristics;
    };

    template<typename T>
    class section_iterator_t
    {
    private:
        T* m_base = nullptr;

        pe_crt::uint16_t m_num_sections = 0;

    public:
        section_iterator_t(T* base, pe_crt::uint16_t num_sections) : m_base(base), m_num_sections(num_sections)
        {

        }

        T* begin() const
        {
            return this->m_base;
        }

        T* end() const
        {
            return this->m_base + this->m_num_sections;
        }
    };
}