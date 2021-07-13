#pragma once

#include <QMimeType>
#include <QMimeDatabase>

namespace sf
{

class AppModuleFileType
{
	public:
		explicit AppModuleFileType(const QMimeType& mime);

		explicit AppModuleFileType(const QString& mime);

		explicit AppModuleFileType(const QString& name, const QString& suffix);

		[[nodiscard]] QString getName() const;

		[[nodiscard]] QString getSuffix() const;

		[[nodiscard]] bool isMime(const QMimeType& mime) const;

		[[nodiscard]] bool isSuffix(const QString& suffix) const;

	private:
		QMimeType _mimeType;
		QString _suffix;
		QString _name;
};

}