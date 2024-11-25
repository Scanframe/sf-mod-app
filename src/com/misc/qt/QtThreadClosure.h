#pragma once
#include <QThread>
#include <misc/gen/TClosure.h>

namespace sf
{

/**
 * @brief Class for creating Qt threads easier.
 */
class QtThreadClosure
	: public QThread
	, public TClosure<void, QThread&, QObject*>
{
	public:
		/**
		 * @brief Standard QT constructor.
		 */
		explicit QtThreadClosure(QObject* parent = nullptr);

		/**
		 * @brief Constructor assigning the closure function.
		 */
		explicit QtThreadClosure(const func_type& f, QObject* parent = nullptr);

		~QtThreadClosure() override;

	protected:
		/**
		 * @brief Overrides run function and calls the closure assigned one.
		 */
		void run() override;
};

inline QtThreadClosure::QtThreadClosure(QObject* parent)
	: QThread(parent)
	, TClosure<void, QThread&, QObject*>()
{
}

inline QtThreadClosure::QtThreadClosure(const func_type& f, QObject* parent)
	: QThread(parent)
	, TClosure<void, QThread&, QObject*>(f)
{
}

inline QtThreadClosure::~QtThreadClosure()
{
	quit();
	requestInterruption();
	wait();
}

inline void QtThreadClosure::run()
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

}// namespace sf