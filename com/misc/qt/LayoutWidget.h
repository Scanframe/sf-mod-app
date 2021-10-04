#pragma once

#include <QWidget>
#include <QDir>
#include "PropertySheetDialog.h"
#include "../global.h"

namespace sf
{

/**
 * Base class for loading Widgets from a ui-file when used in an editor.
 */
class _MISC_CLASS LayoutWidget :public QWidget
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor as QWidget.
		 */
		explicit LayoutWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

		/**
		 * @brief Opens a property editor dialog for the given target in a derived class.
		 *
		 * Te returned pointer can be used to execute the dialog in modal mode.
		 *
		 * @param target Object targeted in the editor.
		 * @return return Pointer to sheet dialog.
		 */
		virtual void openPropertyEditor(QObject* target);

		/**
		 * @brief Executes a popup from .
		 *
		 * @param target Object targeted in the editor.
		 * @param pos Global position to popup the menu.
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

		/**
		 * @brief Gets the directory of the loaded layout file.
		 */
		 [[nodiscard]] virtual QDir getDirectory() const;

		/**
		 * @brief Gets suffix used for layout files.
		 */
		static QString getSuffix();

	private:
		/**
		 * @brief Holds the readonly flag.
		 */
		bool _readOnly{true};
};

}

