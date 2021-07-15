#pragma once

#include <QWidget>
#include "../global.h"

namespace sf
{

// Forward definition.
class PropertySheetDialog;

/**
 * @brief Widget to create a page in a PropertySheet dialog.
 */
class _MISC_CLASS PropertyPage :public QWidget
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 *
		 * @param parent
		 */
		explicit PropertyPage(PropertySheetDialog* parent);

		/**
		 * @brief Gets the name of the page for displaying in the list view.
		 *
		 * When not reimplemented this method returns the Resource settings icon.
		 */
		[[nodiscard]] virtual QIcon getPageIcon() const;

		/**
		 * @brief Gets the name of the page for displaying in the list view.
		 *
		 * When reimplemented the QObject's name is returned.
		 */
		[[nodiscard]] virtual QString getPageName() const;

		/**
		 * @brief Gets the description for hinting of the page in the list view.
		 */
		[[nodiscard]] virtual QString getPageDescription() const;

		/**
		 * @brief Updates the control widgets of the page.
		 *
		 * This method is called to initialize the control widgets.
		 */
		virtual void updatePage() = 0;

		/**
		 * @brief Gets the modified/changed status of the page.
		 *
		 * @return True when modified.
		 */
		[[nodiscard]] virtual bool isPageModified() const = 0;

		/**
		 * @brief Applies the changes made in this page.
		 */
		virtual void applyPage() = 0;

		/**
		 * @brief Called when all pages are applied.
		 *
		 * Method can be overridden by a derived class to update control widgets
		 * that were changed because of other pages being applied.
		 */
		virtual void afterPageApply() = 0;

	protected:
		/**
		 * @brief Connects all known controls change signals to a handler which enables or disables buttons.
		 *
		 * A derived class should call this method when the is setup and controls are updated.
		 * @return List of not connected control widgets.
		 */
		QWidgetList connectControls();
};

}
