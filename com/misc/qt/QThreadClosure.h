#pragma once

#include "../gen/TClosure.h"
#include <QThread>

namespace sf
{

class QThreadClosure :public QThread, public TClosure<void, QThread&>
{
	public:
		explicit QThreadClosure(QObject* parent = nullptr)
			:QThread(parent)
			 , TClosure<void, QThread&>()
		{
		}

		explicit QThreadClosure(const func_type& f, QObject* parent = nullptr)
			:QThread(parent)
			 , TClosure<void, QThread&>(f)
		{
		}

	protected:
		/**
		 * Overrides run function and calls the closure assigned one.
		 */
		void run() override
		{
			if (isAssigned())
			{
				call(*this);
			}
		}
};

}