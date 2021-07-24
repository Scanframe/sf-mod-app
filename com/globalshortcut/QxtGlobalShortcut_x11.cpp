#include "QxtGlobalShortcutPrivate.h"
/****************************************************************************
** Copyright (c) 2006 - 2011, the LibQxt project.
** See the Qxt AUTHORS file for a list of authors and copyright holders.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the LibQxt project nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** <http://libqxt.org>  <foundation@libqxt.org>
*****************************************************************************/

#include <QApplication>
#include <QtGui/6.1.2/QtGui/qpa/qplatformnativeinterface.h>
#include <xcb/xcb.h>
#include <QVector>
#include <QWidget>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <iostream>
#include "xcbkeyboard.h"

namespace
{

/**
 * Creates first invisible application window
 * so X11 key press events can be received.
 *
 * This is used for Cinnamon and KDE.
 */
void createFirstWindow()
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
}

QVector<quint32> maskModifiers()
{
	return QVector<quint32>() << 0 << Mod2Mask << LockMask << (Mod2Mask | LockMask);
}

using X11ErrorHandler = int (*)(Display* display, XErrorEvent* event);

class QxtX11ErrorHandler
{
	public:
		static bool error;

		static int qxtX11ErrorHandler(Display* display, XErrorEvent* event)
		{
			Q_UNUSED(display);
			switch (event->error_code)
			{
				case BadAccess:
				case BadValue:
				case BadWindow:
					if (event->request_code == 33 /* X_GrabKey */ ||
						event->request_code == 34 /* X_UngrabKey */)
					{
						error = true;
					}
			}
			return 0;
		}

		QxtX11ErrorHandler()
			:_previousErrorHandler(nullptr)
		{
			error = false;
			_previousErrorHandler = XSetErrorHandler(qxtX11ErrorHandler);
		}

		~QxtX11ErrorHandler()
		{
			XSetErrorHandler(_previousErrorHandler);
		}

	private:
		X11ErrorHandler _previousErrorHandler;
};

bool QxtX11ErrorHandler::error = false;

class QxtX11Data
{
	public:
		QxtX11Data()
			:_display(nullptr)
		{
			createFirstWindow();
			auto native = QApplication::platformNativeInterface();
			auto display = native->nativeResourceForScreen(QByteArray("display"), QGuiApplication::primaryScreen());
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
		Display* _display;
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

} // namespace

bool QxtGlobalShortcutPrivate::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
	Q_UNUSED(result);

	xcb_key_press_event_t* kev = nullptr;

	if (eventType == "xcb_generic_event_t")
	{
		auto ev = static_cast<xcb_generic_event_t*>(message);
		if ((ev->response_type & 127) == XCB_KEY_PRESS)
		{
			kev = static_cast<xcb_key_press_event_t*>(message);
		}
	}
	if (kev)
	{
		quint32 key_code = kev->detail;
		quint32 key_state = 0;
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
		//
		activateShortcut(key_code, key_state & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask));
	}
	return false;
}

quint32 QxtGlobalShortcutPrivate::nativeModifiers(Qt::KeyboardModifiers modifiers)
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
	//if (modifiers & Qt::MetaModifier)
	//if (modifiers & Qt::KeypadModifier)
	//if (modifiers & Qt::GroupSwitchModifier)
	return native;
}

quint32 QxtGlobalShortcutPrivate::nativeKeycode(Qt::Key key)
{
	QxtX11Data x11;
	if (!x11.isValid())
	{
		return 0;
	}

	const KeySym keysym = qtKeyToXKeySym(key);
	return XKeysymToKeycode(x11.display(), keysym);
}

bool QxtGlobalShortcutPrivate::registerShortcut(quint32 nativeKey, quint32 nativeMods)
{
	QxtX11Data x11;
	return x11.isValid() && x11.grabKey(nativeKey, nativeMods, x11.rootWindow());
}

bool QxtGlobalShortcutPrivate::unregisterShortcut(quint32 nativeKey, quint32 nativeMods)
{
	QxtX11Data x11;
	return x11.isValid() && x11.ungrabKey(nativeKey, nativeMods, x11.rootWindow());
}
