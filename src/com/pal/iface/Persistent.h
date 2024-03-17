#pragma once

#include <QStringList>
#include <QObject>

namespace sf
{

class Persistent
{
	public:
		explicit Persistent(QObject* obj);

		[[nodiscard]] QStringList getProperties() const;

		void setProperties(const QStringList&) const;

	private:
		QObject* _object;
};

}
