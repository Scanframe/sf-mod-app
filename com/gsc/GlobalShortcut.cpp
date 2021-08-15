#include <QApplication>
#include <QTimer>
#include <QHash>
#include <QAbstractNativeEventFilter>
#include <QAbstractEventDispatcher>
#include <iostream>
#include "GlobalShortcut.h"
#include "GlobalShortcutPrivate.h"

namespace sf
{

void GlobalShortcut::Private::installEventFilter(bool install)
{
	static GlobalShortcut::Private::EventFilter filter;
	static int count = 0;
	if (install)
	{
		if (!count++)
		{
			Q_UNUSED(count);
			//std::clog << __FUNCTION__ << " " << install << std::endl;
			QAbstractEventDispatcher::instance()->installNativeEventFilter(&filter);
		}
	}
	else
	{
		if (!--count)
		{
			//std::clog << __FUNCTION__ << " " << install << std::endl;
			QAbstractEventDispatcher::instance()->removeNativeEventFilter(&filter);
		}
	}
}

QHash<QPair<quint32, quint32>, GlobalShortcut*> GlobalShortcut::Private::_shortcuts;

GlobalShortcut::Private::Private(GlobalShortcut* gsc)
	:_enabled(true)
	 , _keyCombination()
	 , _nativeKey(0)
	 , _nativeMods(0)
	 , _registered(false)
	 , _global(false)
	 , _autoRepeat(false)
	 , _ptr(gsc)
{
	// Install the filter if it has not already.
	installEventFilter(true);
}

GlobalShortcut::Private::~Private()
{
	// Unset shortcut by passing the default unknown key combination.
	setShortcut();
	// Uninstall the filter if it has not already.
	installEventFilter(false);
}

bool GlobalShortcut::Private::doRegister(bool flag)
{
	if (_registered != flag)
	{
		// When registered.
		if (_registered)
		{
			_registered = false;
			// Unregister the key.
			return unregisterShortcut(_nativeKey, _nativeMods);
		}
		else
		{
			// Register the key and set the registered flag to true when successful.
			_registered = registerShortcut(_nativeKey, _nativeMods);
			return _registered;
		}
	}
	return true;
}

bool GlobalShortcut::Private::setShortcut(const QKeyCombination& key)
{
	// If no change skip it.
	if (key == _keyCombination)
	{
		return true;
	}
	// Check if the shortcut is to be unset.
	if (key.key() == Qt::Key_unknown)
	{
		// When this instance in responsible.
		if (_shortcuts.value(qMakePair(_nativeKey, _nativeMods)) == _ptr)
		{
			// Remove this entry from the list.
			_shortcuts.remove(qMakePair(_nativeKey, _nativeMods));
			// Could be registered even when not global.
			doRegister(false);
		}
	}
	else
	{
		// Unregister the current entry.
		doRegister(false);
		// Assign the new key.
		_keyCombination = key;
		// Convert to native key code and modifier.
		_nativeKey = nativeKeycode(_keyCombination.key());
		_nativeMods = nativeModifiers(_keyCombination.keyboardModifiers());
		// Add this instance to the list of shortcuts.
		_shortcuts.insert(qMakePair(_nativeKey, _nativeMods), _ptr);
		// When global the key needs to be registered.
		return doRegister(_global);
	}
	return true;
}

void GlobalShortcut::Private::activateShortcut(quint32 nativeKey, quint32 nativeMods, bool repeat)
{
	// Raising the application window is not working using a shortcut activation when handled immediately in Linux so delay it using a single shot timer.
	if (auto sc = _shortcuts.value(qMakePair(nativeKey, nativeMods)))
	{
		if (sc->isEnabled() && sc->autoRepeat() ? true : !repeat)
		{
			QTimer::singleShot(0, [nativeKey, nativeMods]()
			{
				if (GlobalShortcut* shortcut = _shortcuts.value(qMakePair(nativeKey, nativeMods)))
				{
					emit shortcut->activated(shortcut);
				}
			});
		}
	}
}

GlobalShortcut::GlobalShortcut(QObject* parent)
	:QObject(parent)
	 , _p(new Private(this))
{
}

GlobalShortcut::GlobalShortcut(const QKeySequence& sequence, QObject* parent)
	:GlobalShortcut(parent)
{
	setSequence(sequence);
}

GlobalShortcut::~GlobalShortcut()
{
	delete _p;
}

QKeySequence GlobalShortcut::sequence() const
{
	return {_p->_keyCombination};
}

bool GlobalShortcut::setSequence(const QKeySequence& sequence)
{
	return _p->setShortcut(sequence.isEmpty() ? QKeyCombination() : sequence[0]);
}

bool GlobalShortcut::isEnabled() const
{
	return _p->_enabled;
}

bool GlobalShortcut::isValid() const
{
	return _p->_global == _p->_registered;
}

void GlobalShortcut::setEnabled(bool enabled)
{
	_p->_enabled = enabled;
}

void GlobalShortcut::setDisabled(bool disabled)
{
	_p->_enabled = !disabled;
}

void GlobalShortcut::setGlobal(bool global)
{
	if (_p->_global != global)
	{
		if (global)
		{
			// When registration fails the key is not global.
			_p->_global = _p->doRegister(true);
		}
		else
		{
			_p->doRegister(false);
			_p->_global = false;
		}
	}
}

bool GlobalShortcut::isGlobal() const
{
	return _p->_global;
}

bool GlobalShortcut::autoRepeat() const
{
	return _p->_autoRepeat;
}

void GlobalShortcut::setAutoRepeat(bool on)
{
	if (_p->_autoRepeat != on)
	{
		_p->_autoRepeat = on;
// Windows enables repetition of a hotkey with the registration function.
#ifdef Q_OS_WIN
		// So when registered reregister to make the change have effect.
		if (_p->_registered)
		{
			_p->doRegister(false);
			_p->doRegister(true);
		}
#endif
	}
}

}
