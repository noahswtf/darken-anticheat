#include "data_directory.hpp"

bool portable_executable::data_directory_t::present() const
{
	return this->size != 0;
}
