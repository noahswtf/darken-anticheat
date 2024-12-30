#include "relocations_directory.hpp"

void portable_executable::relocations_iterator_t::load_block(const raw_relocation_block_descriptor_t* raw_relocation_block_descriptor)
{
    this->current_raw_relocation_block_descriptor = raw_relocation_block_descriptor;

    if (this->current_raw_relocation_block_descriptor && this->current_raw_relocation_block_descriptor->virtual_address)
    {
        const pe_crt::uint64_t real_block_size = this->current_raw_relocation_block_descriptor->size_of_block - sizeof(raw_relocation_block_descriptor_t);

        this->current_relocation_block.max_entry_index = static_cast<pe_crt::uint32_t>(real_block_size / sizeof(relocation_entry_descriptor_t));
        this->current_relocation_block.current_entry_index = 1;

        this->current_descriptor = reinterpret_cast<const relocation_entry_descriptor_t*>(this->current_raw_relocation_block_descriptor + 1);
    }
}

portable_executable::relocations_iterator_t::relocations_iterator_t(const raw_relocation_block_descriptor_t* raw_relocation_block_descriptor)
{
    if (raw_relocation_block_descriptor)
    {
        this->load_block(raw_relocation_block_descriptor);
    }
}

portable_executable::relocations_iterator_t::value_type portable_executable::relocations_iterator_t::operator*() const
{
    return { *this->current_descriptor, this->current_raw_relocation_block_descriptor->virtual_address };
}

portable_executable::relocations_iterator_t& portable_executable::relocations_iterator_t::operator++()
{
    if (this->current_descriptor && this->current_relocation_block.current_entry_index < this->current_relocation_block.max_entry_index)
    {
        ++this->current_descriptor;

        this->current_relocation_block.current_entry_index++;
    }
    else if (this->current_raw_relocation_block_descriptor && this->current_raw_relocation_block_descriptor->virtual_address)
    {
        this->load_block(reinterpret_cast<const raw_relocation_block_descriptor_t*>(reinterpret_cast<pe_crt::uint64_t>(this->current_raw_relocation_block_descriptor) + this->current_raw_relocation_block_descriptor->size_of_block));
    }
    else
    {
        this->current_descriptor = nullptr;
    }

    return *this;
}

bool portable_executable::relocations_iterator_t::operator==(const relocations_iterator_t& other)
{
    return this->current_descriptor == other.current_descriptor;
}

bool portable_executable::relocations_iterator_t::operator!=(const relocations_iterator_t& other)
{
    return this->current_descriptor != other.current_descriptor;
}
