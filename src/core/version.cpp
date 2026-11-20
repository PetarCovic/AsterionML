#include <ml/core/version.hpp>

namespace ml
{
	std::string_view version() noexcept
	{
		return ASTERIONML_VERSION;
	}
}	//namespace ml