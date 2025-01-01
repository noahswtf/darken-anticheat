#pragma once
#include <generic_types.h>

#pragma warning(push)
#pragma warning(disable: 4201)

union s_virtual_address
{
    uint64_t address;

    struct
    {
        uint64_t offset : 12;
        uint64_t pt_idx : 9;
        uint64_t pd_idx : 9;
        uint64_t pdpt_idx : 9;
        uint64_t pml4_idx : 9;
        uint64_t reserved : 16; // could add support for 5 level paging in future
    };
};

#pragma warning(pop)
