#include "IFileMapper.h"
#include "target.h"

#if IS_WIN
#  include <windows.h>
#  include "win/FileMapper.h"
#else
#  include "lnx/FileMapper.h"
#endif
#if IS_QT
#  include "qt/FileMapper.h"
#endif

namespace sf
{

IFileMapper* IFileMapper::instantiate(bool native)
{
#if IS_QT
	if (!native)
	{
		return new qt::FileMapper;
	}
	else
#endif
	{
#if IS_WIN
		return new win::FileMapper;
#else
		return new lnx::FileMapper;
#endif
	}
}

}
