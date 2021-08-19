#pragma once

#include <QWidget>
#include "../global.h"

namespace sf
{

/**
 * Base class for loading Widgets from a ui-file when used in an editor.
 */
class _MISC_CLASS LayoutWidget :public QWidget
{
	public:
		/**
		 * @brief Constructor as QWidget.
		 */
		explicit LayoutWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

		/**
		 * @brief Opens a property editor dialog for the given target in a derived class.
		 *
		 * @param target Object targeted in the editor.
		 */
		virtual void openPropertyEditor(QObject* target);

		/**
		 * @brief Executes a popup from .
		 *
		 * @param target Object targeted in the editor.
		 */
		virtual void popupContextMenu(QObject* target, const QPoint& pos);

		/**
		 * @brief Property 'converted' set method.
		 */
		void setReadOnly(bool);

		/**
		 * @brief Property 'converted' get method.
		 */
		[[nodiscard]] bool getReadOnly() const;

		/**
		 * @brief Gets the layout widget instance for the passed target if any.
		 *
		 * @param target Target object in the layout.
		 * @return On not found it returns a nullptr.
		 */
		static LayoutWidget* getLayoutWidgetOf(QObject* target);

	private:
		/**
		 * @brief Holds the readonly flag.
		 */
		bool _readOnly{true};
};

}

