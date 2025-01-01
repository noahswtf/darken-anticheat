#include "memory.h"
#include "page_tables.h"

uint64_t memory::translate_virtual_address(s_virtual_address virtual_address, cr3 directory_table_base)
{
	if (virtual_address.address == 0 || directory_table_base.flags == 0)
	{
		return 0;
	}

	pml4e_64* pml4 = reinterpret_cast<pml4e_64*>(page_tables::pt_access_virtual_address + (directory_table_base.address_of_page_directory << 12));

	pml4e_64 pml4e = pml4[virtual_address.pml4_idx];

	if (pml4e.present == 0)
	{
		return 0;
	}

	pdpte_64* pdpt = reinterpret_cast<pdpte_64*>(page_tables::pt_access_virtual_address + (pml4e.page_frame_number << 12));

	pdpte_64 pdpte = pdpt[virtual_address.pdpt_idx];

	if (pdpte.present == 0)
	{
		return 0;
	}

	if (pdpte.large_page == 1)
	{
		pdpte_1gb_64 pdpte_1gb = { };

		pdpte_1gb.flags = pdpte.flags;

		return virtual_address.offset + (virtual_address.pt_idx << 12) + (virtual_address.pd_idx << 21) + (pdpte_1gb.page_frame_number << 30);
	}

	pde_64* pd = reinterpret_cast<pde_64*>(page_tables::pt_access_virtual_address + (pdpte.page_frame_number << 12));

	pde_64 pde = pd[virtual_address.pd_idx];

	if (pde.present == 0)
	{
		return 0;
	}

	if (pde.large_page == 1)
	{
		pde_2mb_64 pde_2mb = { };

		pde_2mb.flags = pde.flags;

		return virtual_address.offset + (virtual_address.pt_idx << 12) + (pde_2mb.page_frame_number << 21);
	}

	pte_64* pt = reinterpret_cast<pte_64*>(page_tables::pt_access_virtual_address + (pde.page_frame_number << 12));

	pte_64 pte = pt[virtual_address.pt_idx];

	if (pte.present == 0)
	{
		return 0;
	}

	return virtual_address.offset + (pte.page_frame_number << 12);
}

bool memory::is_address_valid(uint64_t virtual_address, uint64_t directory_table_base)
{
	return translate_virtual_address({ virtual_address }, { .flags = directory_table_base }) != 0;
}
