#include <ml/core/version.hpp>

#include <iostream>

int main()
{
	std::cout << "AsterionML version: " << ml::version() << '\n';

	return 0;
}