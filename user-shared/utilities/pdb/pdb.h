#pragma once
#include <string>

struct s_pdb_info
{
    std::string pdb_name;
    std::string guid;
    std::string age;
};

namespace utilities
{
    namespace pdb
    {
        s_pdb_info get_pdb_info(std::wstring_view binary_path);
    }
}
