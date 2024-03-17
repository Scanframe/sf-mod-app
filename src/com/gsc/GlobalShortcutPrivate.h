#pragma once

#include <QAbstractEventDispatcher>
#include <QHash>
#include <QAbstractNativeEventFilter>
#include "GlobalShortcut.h"

namespace sf
{

struct GlobalShortcut::Private
{
	explicit Private(GlobalShortcut* q);

	~Private();

	bool setShortcut(const QKeyCombination& key = QKeyCombination());

	bool doRegister(bool flag);

	QKeyCombination _keyCombination;
	bool _enabled;
	quint32 _nativeKey;
	quint32 _nativeMods;
	bool _registered;
	bool _global;
	bool _autoRepeat;
	GlobalShortcut* _ptr;

	// Looks up the shortcut and activates it.
	static void activateShortcut(quint32 nativeKey, quint32 nativeMods, bool repeat);

	// Global storage of all created shortcuts.
	static QHash<QPair<quint32, quint32>, GlobalShortcut*> _shortcuts;

	// All methods here are OS dependent and in separate files for each OS another one.
	struct EventFilter :QAbstractNativeEventFilter
	{
		bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
	};

	quint32 nativeKeycode(Qt::Key keycode);

	quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);

	bool registerShortcut(quint32 nativeKey, quint32 nativeMods);

	bool unregisterShortcut(quint32 nativeKey, quint32 nativeMods);

	// Install or uninstalls the event filter.
	// Keeps count of the amount of calls.
	static void installEventFilter(bool install);
};

}
