#include <gen/Variable.h>
#include <gen/ResultData.h>

__attribute__((constructor)) void LibraryInitialize()
{
	sf::Variable::initialize();
	sf::ResultData::initialize();
}

__attribute__((destructor)) void LibraryDeinitialize()
{
	sf::Variable::deinitialize();
	sf::ResultData::deinitialize();
}
