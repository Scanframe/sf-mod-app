#pragma once

#include "../gen/TClosure.h"
#include <QThread>

namespace sf
{

/**
 * @brief Class for creating Qt threads easier.
 */
class QtThreadClosure :public QThread, public TClosure<void, QThread&, QObject*>
{
	public:
		/**
		 * @brief Standard QT constructor.
		 */
		explicit QtThreadClosure(QObject* parent = nullptr)
			:QThread(parent)
			 , TClosure<void, QThread&, QObject*>()
		{
		}

		/**
		 * @brief Constructor assigning the closure function.
		 */
		explicit QtThreadClosure(const func_type& f, QObject* parent = nullptr)
			:QThread(parent)
			 , TClosure<void, QThread&, QObject*>(f)
		{
		}

		~QtThreadClosure() override
		{
			quit();
			requestInterruption();
			wait();
		}

	protected:
		/**
		 * @brief Overrides run function and calls the closure assigned one.
		 */
		void run() override
		{
			// Only call when assigned.
			if (isAssigned())
			{
				// Object serves a parent for objects created in the thread itself.
				QObject threadParent;
				// Run the thread function/method.
				call(*this, &threadParent);
			}
		}
};

}