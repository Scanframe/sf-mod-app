#include "AppModuleFileType.h"

namespace sf
{

AppModuleFileType::AppModuleFileType(const QString& name, const QString& suffix)
	:_name(name)
	 ,_suffix(suffix)
{}

AppModuleFileType::AppModuleFileType(const QString& mime)
	:_mimeType(QMimeDatabase().mimeTypeForName(mime))
{}

AppModuleFileType::AppModuleFileType(const QMimeType& mime)
	:_mimeType(mime)
{}

QString AppModuleFileType::getName() const
{
	return _mimeType.isValid() ? _mimeType.comment() : _name;
}

QString AppModuleFileType::getSuffix() const
{
	return _mimeType.isValid() ? _mimeType.preferredSuffix() : _suffix;
}

bool AppModuleFileType::isMime(const QMimeType& mime) const
{
	return _mimeType == mime;
}

bool AppModuleFileType::isSuffix(const QString& suffix) const
{
	return (_mimeType.isValid() ? _mimeType.preferredSuffix() : _suffix) == suffix;
}

}
