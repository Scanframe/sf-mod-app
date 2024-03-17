#include "GlobalShortcutPrivate.h"
#include <qt_windows.h>

namespace sf
{

bool GlobalShortcut::Private::EventFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
	Q_UNUSED(eventType);
	Q_UNUSED(result);
	MSG* msg = static_cast<MSG*>(message);
	// When registered as a global hot key.
	if (msg->message == WM_HOTKEY)
	{
		const quint32 keycode = HIWORD(msg->lParam);
		const quint32 modifiers = LOWORD(msg->lParam);
		activateShortcut(keycode, modifiers, false);
	}
	// When as a local key down.
	if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN)
	{
		const quint32 keycode = msg->wParam;
		quint32 modifiers = 0;
		const auto flags = HIWORD(msg->lParam);
		if (GetKeyState(VK_SHIFT) < 0)
		{
			modifiers |= MOD_SHIFT;
		}
		if (GetKeyState(VK_CONTROL) < 0)
		{
			modifiers |= MOD_CONTROL;
		}
		if (GetKeyState(VK_LWIN) < 0 || (GetKeyState(VK_RWIN) < 0))
		{
			modifiers |= MOD_WIN;
		}
		if (flags & KF_ALTDOWN)
		{
			modifiers |= MOD_ALT;
		}
		activateShortcut(keycode, modifiers, flags & KF_REPEAT);
	}
	return false;
}

quint32 GlobalShortcut::Private::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
	// MOD_ALT, MOD_CONTROL, (MOD_KEYUP), MOD_SHIFT, MOD_WIN
	quint32 native = 0;
	if (modifiers & Qt::ShiftModifier)
	{
		native |= MOD_SHIFT;
	}
	if (modifiers & Qt::ControlModifier)
	{
		native |= MOD_CONTROL;
	}
	if (modifiers & Qt::AltModifier)
	{
		native |= MOD_ALT;
	}
	if (modifiers & Qt::MetaModifier)
	{
		native |= MOD_WIN;
	}
	// TODO: resolve these?
	//if (modifiers & Qt::KeypadModifier)
	//if (modifiers & Qt::GroupSwitchModifier)
	return native;
}

bool GlobalShortcut::Private::registerShortcut(quint32 nativeKey, quint32 nativeMods) // NOLINT(readability-make-member-function-const)
{
	return RegisterHotKey(nullptr, (int)(nativeMods ^ nativeKey), _autoRepeat ? nativeMods : nativeMods | MOD_NOREPEAT, nativeKey);
}

bool GlobalShortcut::Private::unregisterShortcut(quint32 nativeKey, quint32 nativeMods)
{
	return UnregisterHotKey(nullptr, (int)(nativeMods ^ nativeKey));
}

quint32 GlobalShortcut::Private::nativeKeycode(Qt::Key key)
{
	// Here is list of keys that presumably work on most keyboard layouts.
	// Default branch is for keys that can change with keyboard layout.
	switch (key)
	{
		case Qt::Key_Escape:
			return VK_ESCAPE;
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			return VK_TAB;
		case Qt::Key_Backspace:
			return VK_BACK;
		case Qt::Key_Return:
		case Qt::Key_Enter:
			return VK_RETURN;
		case Qt::Key_Insert:
			return VK_INSERT;
		case Qt::Key_Delete:
			return VK_DELETE;
		case Qt::Key_Pause:
			return VK_PAUSE;
		case Qt::Key_Print:
			return VK_PRINT;
		case Qt::Key_Clear:
			return VK_CLEAR;
		case Qt::Key_Home:
			return VK_HOME;
		case Qt::Key_End:
			return VK_END;
		case Qt::Key_Left:
			return VK_LEFT;
		case Qt::Key_Up:
			return VK_UP;
		case Qt::Key_Right:
			return VK_RIGHT;
		case Qt::Key_Down:
			return VK_DOWN;
		case Qt::Key_PageUp:
			return VK_PRIOR;
		case Qt::Key_PageDown:
			return VK_NEXT;
		case Qt::Key_F1:
			return VK_F1;
		case Qt::Key_F2:
			return VK_F2;
		case Qt::Key_F3:
			return VK_F3;
		case Qt::Key_F4:
			return VK_F4;
		case Qt::Key_F5:
			return VK_F5;
		case Qt::Key_F6:
			return VK_F6;
		case Qt::Key_F7:
			return VK_F7;
		case Qt::Key_F8:
			return VK_F8;
		case Qt::Key_F9:
			return VK_F9;
		case Qt::Key_F10:
			return VK_F10;
		case Qt::Key_F11:
			return VK_F11;
		case Qt::Key_F12:
			return VK_F12;
		case Qt::Key_F13:
			return VK_F13;
		case Qt::Key_F14:
			return VK_F14;
		case Qt::Key_F15:
			return VK_F15;
		case Qt::Key_F16:
			return VK_F16;
		case Qt::Key_F17:
			return VK_F17;
		case Qt::Key_F18:
			return VK_F18;
		case Qt::Key_F19:
			return VK_F19;
		case Qt::Key_F20:
			return VK_F20;
		case Qt::Key_F21:
			return VK_F21;
		case Qt::Key_F22:
			return VK_F22;
		case Qt::Key_F23:
			return VK_F23;
		case Qt::Key_F24:
			return VK_F24;
		case Qt::Key_Space:
			return VK_SPACE;
		case Qt::Key_Asterisk:
			return VK_MULTIPLY;
		case Qt::Key_Plus:
			return VK_ADD;
		case Qt::Key_Minus:
			return VK_SUBTRACT;
		case Qt::Key_Slash:
			return VK_DIVIDE;
		case Qt::Key_MediaNext:
			return VK_MEDIA_NEXT_TRACK;
		case Qt::Key_MediaPrevious:
			return VK_MEDIA_PREV_TRACK;
		case Qt::Key_MediaPlay:
			return VK_MEDIA_PLAY_PAUSE;
		case Qt::Key_MediaStop:
			return VK_MEDIA_STOP;
			// couldn't find those in VK_*
			//case Qt::Key_MediaLast:
			//case Qt::Key_MediaRecord:
		case Qt::Key_VolumeDown:
			return VK_VOLUME_DOWN;
		case Qt::Key_VolumeUp:
			return VK_VOLUME_UP;
		case Qt::Key_VolumeMute:
			return VK_VOLUME_MUTE;

		default:
			// Try to get virtual key from current keyboard layout or US.
			HKL layout = GetKeyboardLayout(0);
			int vk = VkKeyScanEx(key, layout);
			if (vk == -1)
			{
				HKL layoutUs = GetKeyboardLayout(0x409);
				vk = VkKeyScanEx(key, layoutUs);
			}
			return vk == -1 ? 0 : vk;
	}
}

}
