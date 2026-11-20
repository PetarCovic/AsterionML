#include <ml/core/version.hpp>

#include <iostream>
#include <ranges>
#include <span>

int main()
{
	const auto reported_version = ml::version();

	if (reported_version != ASTERIONML_EXPECTED_VERSION)
	{
		std::cerr << "Version mismatch: reported version is " << reported_version
			<< ", but the expected version is " << ASTERIONML_EXPECTED_VERSION << '\n';
		return 1;
	}

	std::cout << "Build and link smoke test passed. AsterionML version: " << reported_version << '\n';

	return 0;
}

static_assert(
	std::ranges::contiguous_range<std::span<const double>>,
	"The test consumer must support C++20 ranges and spans.");