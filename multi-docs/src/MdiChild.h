#pragma once

#include <QString>

class MdiChild
{
	public:
		virtual void newFile() = 0;

		virtual bool loadFile(const QString& fileName) = 0;

		virtual bool save() = 0;

		virtual bool saveAs() = 0;

		virtual bool saveFile(const QString& fileName) = 0;

		virtual QString userFriendlyCurrentFile() const = 0;

		virtual QString currentFile() const = 0;

		virtual bool hasSelection() const = 0;

#ifndef QT_NO_CLIPBOARD

		virtual void cut() = 0;

		virtual void copy() = 0;

		virtual void paste() = 0;

#endif
};
