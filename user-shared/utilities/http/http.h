#pragma once
#include <string>

namespace http
{
	bool does_url_exist(std::string_view host, std::string_view target);
}
