#pragma once

#include <QAbstractEventDispatcher>
#include <QHash>
#include <QAbstractNativeEventFilter>

class QKeySequence;

class QxtGlobalShortcut;

class QxtGlobalShortcutPrivate :public QAbstractNativeEventFilter
{
	public:
		explicit QxtGlobalShortcutPrivate(QxtGlobalShortcut* q);

		~QxtGlobalShortcutPrivate() override;

		bool _enabled;
		Qt::Key _key;
		Qt::KeyboardModifiers _mods;
		quint32 _nativeKey;
		quint32 _nativeMods;
		bool _registered;

		bool setShortcut(const QKeySequence& sequence);

		bool unsetShortcut();

#ifndef Q_OS_MAC
		static int ref;
#endif // Q_OS_MAC

		bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

		static void activateShortcut(quint32 nativeKey, quint32 nativeMods);

	private:
		QxtGlobalShortcut* q_ptr;

		static quint32 nativeKeycode(Qt::Key keycode);

		static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);

		static bool registerShortcut(quint32 nativeKey, quint32 nativeMods);

		static bool unregisterShortcut(quint32 nativeKey, quint32 nativeMods);

		static QHash<QPair<quint32, quint32>, QxtGlobalShortcut*> shortcuts;
};
