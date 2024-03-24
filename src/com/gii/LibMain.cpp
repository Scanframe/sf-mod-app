#include <gii/gen/Variable.h>
#include <gii/gen/ResultData.h>

__attribute__((constructor)) void LibraryInitialize()
{
	sf::Variable::initialize();
	sf::ResultData::initialize();
}

__attribute__((destructor)) void LibraryUninitialize()
{
	sf::Variable::uninitialize();
	sf::ResultData::uninitialize();
}
