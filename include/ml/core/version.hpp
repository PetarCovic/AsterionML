#pragma once

#include <string_view>

namespace ml
{
	[[nodiscard]] std::string_view version() noexcept;
} //namespace ml