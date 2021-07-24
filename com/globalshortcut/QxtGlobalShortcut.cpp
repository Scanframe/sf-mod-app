#include "QxtGlobalShortcut.h"
#include "QxtGlobalShortcutPrivate.h"

#include <QAbstractEventDispatcher>

#ifndef Q_OS_MAC

int QxtGlobalShortcutPrivate::ref = 0;

#endif

QHash<QPair<quint32, quint32>, QxtGlobalShortcut*> QxtGlobalShortcutPrivate::shortcuts;

QxtGlobalShortcutPrivate::QxtGlobalShortcutPrivate(QxtGlobalShortcut* q)
	:_enabled(true)
	 , _key(Qt::Key(0))
	 , _mods(Qt::NoModifier)
	 , _nativeKey(0)
	 , _nativeMods(0)
	 , _registered(false)
	 , q_ptr(q)
{
#ifndef Q_OS_MAC
	if (ref == 0)
	{
		QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
	}
	++ref;
#endif // Q_OS_MAC
}

QxtGlobalShortcutPrivate::~QxtGlobalShortcutPrivate()
{
	unsetShortcut();

#ifndef Q_OS_MAC
	--ref;
	if (ref == 0)
	{
		if (auto ed = QAbstractEventDispatcher::instance())
		{
			ed->removeNativeEventFilter(this);
		}
	}
#endif // Q_OS_MAC
}

bool QxtGlobalShortcutPrivate::setShortcut(const QKeySequence& sequence)
{
	unsetShortcut();

	const Qt::KeyboardModifiers allMods = Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier;

	if (sequence.isEmpty())
	{
		return false;
	}

	_nativeKey = nativeKeycode(sequence[0].key());
	_nativeMods = nativeModifiers(sequence[0].keyboardModifiers());

	_registered = registerShortcut(_nativeKey, _nativeMods);
	if (_registered)
	{
		shortcuts.insert(qMakePair(_nativeKey, _nativeMods), q_ptr);
	}

	return _registered;
}

bool QxtGlobalShortcutPrivate::unsetShortcut()
{
	if (_registered
		&& shortcuts.value(qMakePair(_nativeKey, _nativeMods)) == q_ptr
		&& unregisterShortcut(_nativeKey, _nativeMods))
	{
		shortcuts.remove(qMakePair(_nativeKey, _nativeMods));
		_registered = false;
		return true;
	}

	return false;
}

void QxtGlobalShortcutPrivate::activateShortcut(quint32 nativeKey, quint32 nativeMods)
{
	QxtGlobalShortcut* shortcut = shortcuts.value(qMakePair(nativeKey, nativeMods));
	if (shortcut && shortcut->isEnabled())
	{
		emit shortcut->activated(shortcut);
	}
}

/*!
    \class QxtGlobalShortcut
    \brief The QxtGlobalShortcut class provides a global shortcut aka "hotkey".

    A global shortcut triggers even if the application is not active. This
    makes it easy to implement applications that react to certain shortcuts
    still if some other application is active or if the application is for
    example minimized to the system tray.

    Example usage:
    \code
    QxtGlobalShortcut* shortcut = new QxtGlobalShortcut(window);
    connect(shortcut, SIGNAL(activated()), window, SLOT(toggleVisibility()));
    shortcut->setShortcut(QKeySequence("Ctrl+Shift+F12"));
    \endcode
 */

/*!
    \fn QxtGlobalShortcut::activated()

    This signal is emitted when the user types the shortcut's key sequence.

    \sa shortcut
 */

/*!
    Constructs a new QxtGlobalShortcut with \a parent.
 */
QxtGlobalShortcut::QxtGlobalShortcut(QObject* parent)
	:QObject(parent)
	 , _p(new QxtGlobalShortcutPrivate(this))
{
}

/*!
    Constructs a new QxtGlobalShortcut with \a shortcut and \a parent.
 */
QxtGlobalShortcut::QxtGlobalShortcut(const QKeySequence& sequence, QObject* parent)
	:QxtGlobalShortcut(parent)
{
	setShortcut(sequence);
}

/*!
    Destructs the QxtGlobalShortcut.
 */
QxtGlobalShortcut::~QxtGlobalShortcut()
{
	delete _p;
}

/*!
    \property QxtGlobalShortcut::shortcut
    \brief the shortcut key sequence

    Notice that corresponding key press and release events are not
    delivered for registered global shortcuts even if they are disabled.
    Also, comma separated key sequences are not supported.
    Only the first part is used:

    \code
    qxtShortcut->setShortcut(QKeySequence("Ctrl+Alt+A,Ctrl+Alt+B"));
    Q_ASSERT(qxtShortcut->shortcut() == QKeySequence("Ctrl+Alt+A"));
    \endcode

    \sa setShortcut()
 */
QKeySequence QxtGlobalShortcut::shortcut() const
{
	return {static_cast<QKeyCombination>(_p->_key | _p->_mods)};
}

/*!
    \property QxtGlobalShortcut::shortcut
    \brief sets the shortcut key sequence

    \sa shortcut()
 */
bool QxtGlobalShortcut::setShortcut(const QKeySequence& sequence)
{
	return _p->setShortcut(sequence);
}

/*!
    \property QxtGlobalShortcut::enabled
    \brief whether the shortcut is enabled

    A disabled shortcut does not get activated.

    The default value is \c true.

    \sa setEnabled(), setDisabled()
 */
bool QxtGlobalShortcut::isEnabled() const
{
	return _p->_enabled;
}

/*!
    \property QxtGlobalShortcut::valid
    \brief whether the shortcut was successfully set up
 */
bool QxtGlobalShortcut::isValid() const
{
	return _p->_registered;
}

/*!
    Sets the shortcut \a enabled.

    \sa enabled
 */
void QxtGlobalShortcut::setEnabled(bool enabled)
{
	_p->_enabled = enabled;
}

/*!
    Sets the shortcut \a disabled.

    \sa enabled
 */
void QxtGlobalShortcut::setDisabled(bool disabled)
{
	_p->_enabled = !disabled;
}
