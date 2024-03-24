#include <QApplication>
#include <qpa/qplatformwindow.h>
#include <qpa/qplatformnativeinterface.h>
#include <xcb/xcb.h>
#include <QVector>
#include <QWidget>
#include <X11/Xlib.h>
#include <iostream>
#include "xcbkeyboard.h"
#include "GlobalShortcutPrivate.h"

// Defined in xcb_utils.h but not available here right now.
#ifndef XCB_EVENT_RESPONSE_TYPE_MASK
#  define XCB_EVENT_RESPONSE_TYPE_MASK   (0x7f)
#endif

namespace
{

/**
 * Creates first invisible application window
 * so X11 key press events can be received.
 *
 * This is used for Cinnamon and KDE.
 */
QWidget* createFirstWindow()
{
	static QWidget* w = nullptr;
	if (!w)
	{
		// Try hard so the window is not visible.

		// Tool tips won't show in taskbar.
		w = new QWidget(nullptr, Qt::ToolTip);

		// Move out of screen (if it's not possible to show the window minimized).
		w->resize(1, 1);
		w->move(-100000, -100000);

		// Show and hide quickly.
		w->showMinimized();
		w->hide();
	}
	return w;
}

QVector<quint32> maskModifiers()
{
	return QVector<quint32>() << 0 << Mod2Mask << LockMask << (Mod2Mask | LockMask);
}

class QxtX11ErrorHandler
{
	public:
		// Constructor replacing the X11 error handler.
		QxtX11ErrorHandler()
		{
			// Reset error flag.
			error = false;
			// Replace error handler.
			_previousErrorHandler = XSetErrorHandler(qxtX11ErrorHandler);
			Q_UNUSED(_previousErrorHandler);
		}

		// Destructor returning previous X11 error handler.
		~QxtX11ErrorHandler()
		{
			XSetErrorHandler(_previousErrorHandler);
		}

		// Error handler function type.
		typedef int (* handler)(Display* display, XErrorEvent* event);

		// Flag for error.
		static bool error;

		// Error handler function.
		static int qxtX11ErrorHandler(Display* display, XErrorEvent* event)
		{
			Q_UNUSED(display);
			switch (event->error_code)
			{
				case BadAccess:
				case BadValue:
				case BadWindow:
					if (event->request_code == 33 /* X_GrabKey */ || event->request_code == 34 /* X_UngrabKey */)
					{
						error = true;
					}
			}
			return 0;
		}

	private:
		handler _previousErrorHandler{nullptr};
};

bool QxtX11ErrorHandler::error = false;

class QxtX11Data
{
	public:
		QxtX11Data()
		{
			createFirstWindow();
			auto display = QApplication::platformNativeInterface()->nativeResourceForScreen(QByteArray("display"), QGuiApplication::primaryScreen());
			_display = reinterpret_cast<Display*>(display);
		}

		[[nodiscard]] bool isValid() const
		{
			return _display != nullptr;
		}

		Display* display()
		{
			Q_ASSERT(isValid());
			return _display;
		}

		Window rootWindow()
		{
			return DefaultRootWindow(display());
		}

		bool grabKey(quint32 keycode, quint32 modifiers, Window window)
		{
			QxtX11ErrorHandler errorHandler;
			for (const auto maskMods : maskModifiers())
			{
				XGrabKey(display(), static_cast<int>(keycode), modifiers | maskMods, window, True, GrabModeAsync, GrabModeAsync);
				if (QxtX11ErrorHandler::error)
				{
					break;
				}
			}
			if (QxtX11ErrorHandler::error)
			{
				ungrabKey(keycode, modifiers, window);
				return false;
			}
			return true;
		}

		bool ungrabKey(quint32 keycode, quint32 modifiers, Window window)
		{
			QxtX11ErrorHandler errorHandler;
			for (const auto maskMods : maskModifiers())
			{
				XUngrabKey(display(), static_cast<int>(keycode), modifiers | maskMods, window);
			}
			return !QxtX11ErrorHandler::error;
		}

	private:
		Display* _display{nullptr};
};

KeySym qtKeyToXKeySym(Qt::Key key)
{
	const auto keySym = XStringToKeysym(QKeySequence(key).toString().toLatin1().data());
	if (keySym != NoSymbol)
	{
		return keySym;
	}
	for (int i = 0; KeyTbl[i] != 0; i += 2)
	{
		if (KeyTbl[i + 1] == key)
		{
			return KeyTbl[i];
		}
	}
	return static_cast<ushort>(key);
}

} // end of anonymous namespace

namespace sf
{

bool GlobalShortcut::Private::EventFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
	static xcb_timestamp_t last_time = 0;
	Q_UNUSED(result);
	// Check the event type.
	if (eventType == "xcb_generic_event_t")
	{
		// Cast the pointer to the key event structure.
		auto kev = static_cast<xcb_key_press_event_t*>(message);
		// If the response type is the key pressed.
		if ((kev->response_type & XCB_EVENT_RESPONSE_TYPE_MASK) == XCB_KEY_PRESS)
		{
			quint32 key_code = kev->detail;
			quint32 key_state = 0;
			//
			if (kev->state & XCB_MOD_MASK_1)
			{
				key_state |= Mod1Mask;
			}
			if (kev->state & XCB_MOD_MASK_CONTROL)
			{
				key_state |= ControlMask;
			}
			if (kev->state & XCB_MOD_MASK_4)
			{
				key_state |= Mod4Mask;
			}
			if (kev->state & XCB_MOD_MASK_SHIFT)
			{
				key_state |= ShiftMask;
			}
			activateShortcut(key_code, key_state & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask), (last_time == kev->time));
			// Update static time value for comparison of next key event.
			last_time = kev->time;
			// Prevent warning.
			Q_UNUSED(last_time);
		}
			// When a key is released register the time to compare when next key event has the same time stamp.
		else if ((kev->response_type & XCB_EVENT_RESPONSE_TYPE_MASK) == XCB_KEY_RELEASE)
		{
			last_time = kev->time;
			// Prevent warning.
			Q_UNUSED(last_time);
		}
	}
	return false;
}

quint32 GlobalShortcut::Private::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
	// ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask, Mod4Mask, and Mod5Mask
	quint32 native = 0;
	if (modifiers & Qt::ShiftModifier)
	{
		native |= ShiftMask;
	}
	if (modifiers & Qt::ControlModifier)
	{
		native |= ControlMask;
	}
	if (modifiers & Qt::AltModifier)
	{
		native |= Mod1Mask;
	}
	if (modifiers & Qt::MetaModifier)
	{
		native |= Mod4Mask;
	}
	// TODO: resolve these?
	//if (modifiers & Qt::KeypadModifier)
	//if (modifiers & Qt::GroupSwitchModifier)
	return native;
}

quint32 GlobalShortcut::Private::nativeKeycode(Qt::Key key)
{
	QxtX11Data x11;
	if (!x11.isValid())
	{
		return 0;
	}
	return XKeysymToKeycode(x11.display(), qtKeyToXKeySym(key));
}

bool GlobalShortcut::Private::registerShortcut(quint32 nativeKey, quint32 nativeMods)
{
	QxtX11Data x11;
	if (x11.isValid())
		return x11.grabKey(nativeKey, nativeMods, x11.rootWindow());
	return false;
}

bool GlobalShortcut::Private::unregisterShortcut(quint32 nativeKey, quint32 nativeMods)
{
	QxtX11Data x11;
	return x11.ungrabKey(nativeKey, nativeMods, x11.rootWindow());
}

}
