#pragma once

#include <QWidget>
#include <QDir>
#include <misc/qt/PropertySheetDialog.h>
#include "Namespace.h"
#include "../global.h"

namespace sf
{

/**
 * Base class for loading Widgets from a ui-file when used in an editor for example.
 */
class _GII_CLASS LayoutData :public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor as QWidget.
		 */
		explicit LayoutData(QObject* parent = nullptr);

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
		static LayoutData* from(QObject* target);

		/**
		 * @brief Returns true when the modifier are met for the context menu to popup.
		 */
		static bool hasMenuModifiers();

		/**
		 * @brief Gets the directory of the loaded layout file.
		 */
		 [[nodiscard]] virtual QDir getDirectory() const;

		/**
		 * @brief Gets the directory of the loaded layout file.
		 * Only has effect when #getDirectory() is not overridden.
		 */
		void setDirectory(const QDir& directory);

		/**
		 * @brief Gets suffix used for layout files.
		 */
		static QString getFileSuffix();

		/**
		 * @brief Gets the id offset for all Gii widgets.
		 * Only applied when a layout is loaded.
		 */
		[[nodiscard]] Gii::IdType getIdOffset() const;

		/**
		 * @brief Sets id offset for all Gii widgets.
		 * Only applied when a layout is loaded.
		 */
	void setIdOffset(Gii::IdType idOffset) const;

	private:

		struct Private;
		Private* _p{nullptr};
};

}

