#pragma once

#include <QObject>
#include <QKeySequence>
#include "global.h"

namespace sf
{

/**
 * @class GlobalShortcut
 * @brief The GlobalShortcut class provides a global shortcut aka "hotkey".
 *
 * A global shortcut triggers even if the application is not active. This
 * makes it easy to implement applications that react to certain shortcuts
 * still if some other application is active or if the application is for
 * example minimized to the system tray.
 *
 * Example usage:
 * @code
 * GlobalShortcut* shortcut = new GlobalShortcut(window);
 * connect(shortcut, SIGNAL(activated()), window, SLOT(toggleVisibility()));
 * shortcut->setShortcut(QKeySequence("Ctrl+Shift+F12"));
 * @endcode
 */
class _GSC_CLASS GlobalShortcut :public QObject
	{
		Q_OBJECT
		Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
		Q_PROPERTY(bool valid READ isValid)
		Q_PROPERTY(QKeySequence sequence READ sequence WRITE setSequence)
		Q_PROPERTY(bool global READ isGlobal WRITE setGlobal)

		public:
			/**
			 * @brief Constructs a new GlobalShortcut with @a parent.
			 */
			explicit GlobalShortcut(QObject* parent = nullptr);

			/**
			 * @brief Constructs a new GlobalShortcut with @a sequence and @a parent.
			 */
			explicit GlobalShortcut(const QKeySequence& sequence, QObject* parent = nullptr);

			/**
			 * @brief Destructs the GlobalShortcut.
			 */
			~GlobalShortcut() override;

			/**
			 * @property GlobalShortcut::sequence
			 * @brief the shortcut key sequence
			 *
			 * Notice that corresponding key press and release events are not
			 * delivered for registered global shortcuts even if they are disabled.
			 * Also, comma separated key sequences are not supported.
			 * Only the first part is used:
			 * @code
			 * qxtShortcut->setShortcut(QKeySequence("Ctrl+Alt+A,Ctrl+Alt+B"));
			 * Q_ASSERT(qxtShortcut->sequence() == QKeySequence("Ctrl+Alt+A"));
			 * @endcode
			 *
			 * @see setSequence()
			 */
			[[nodiscard]] QKeySequence sequence() const;

			/**
			 * @property GlobalShortcut::sequence
			 * @brief sets the shortcut key sequence.
			 * @see sequence()
			 */
			bool setSequence(const QKeySequence& sequence);

			/**
			 * @property GlobalShortcut::enabled
			 * @brief Gets whether the shortcut is enabled.
			 *
			 * A disabled shortcut does not get activated.
			 * The default value is @c true.
			 * @see setEnabled(), setDisabled()
			 */
			[[nodiscard]] bool isEnabled() const;

			/**
			 * @property GlobalShortcut::valid
			 * @brief Gets whether the shortcut was successfully set up.
			 */
			[[nodiscard]] bool isValid() const;

			/**
			 * @brief Makes the shortcut grab a global key.
			 *
			 * By default it is false and the shortcuts only respond when the application has focus.
			 * When the key is global the key is processed by this application and no others.
			 */
			void setGlobal(bool global = true);

			/**
			 * @brief Gets if the shortcut only is global.
			 * @return True when global
			 */
			[[nodiscard]] bool isGlobal() const;

			/**
			 * @brief Gets if the shortcut auto repeats.
			 * @return True when auto repeating.
			 */
			[[nodiscard]] bool autoRepeat() const;

			/**
			 * @brief Sets if the shortcut should auto repeat.
			 */
			void setAutoRepeat(bool on);

			/**
			 * @brief Internally used private type.
			 */
			struct Private;

			public Q_SLOTS:

			/**
			 * @brief Sets the shortcut @a enabled.
			 * @see enabled
			 */
			void setEnabled(bool enabled = true);

			/**
			 * @brief Sets the shortcut @a disabled.
			 * @see enabled
			 */
			void setDisabled(bool disabled = true);

			Q_SIGNALS:

		/**
		 * @brief This signal is emitted when the user types the shortcut's key sequence.
		 * @see sequence
		 */
		void activated(GlobalShortcut* self);

		private:
			/**
			 * @brief Holds all the members that are not to be exposed at all.
			 */
			Private* _p;
	};

}