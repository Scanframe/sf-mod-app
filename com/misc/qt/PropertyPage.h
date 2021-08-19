#pragma once

#include <QWidget>
#include <QSettings>
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
		 * @param parent Myst be a PropertySheetDialog pointer or derived one.
		 */
		explicit PropertyPage(QWidget* parent);

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
		virtual void updatePage();

		/**
		 * @brief Gets the modified/changed status of the page.
		 *
		 * @return True when modified.
		 */
		[[nodiscard]] virtual bool isPageModified() const;

		/**
		 * @brief Applies the changes made in this page.
		 */
		virtual void applyPage();

		/**
		 * @brief Called when all pages are applied.
		 *
		 * Method can be overridden by a derived class to update control widgets
		 * that were changed because of other pages being applied.
		 * @param was_modified True when this page was modified.
		 */
		virtual void afterPageApply(bool was_modified);

		/**
		 * @brief Gets the parent sheet of this page. (also the parent but casted.)
		 */
		PropertySheetDialog* getSheet(){return _sheet;};

		/**
		 * @brief Called by sheet to save the state of the page.
		 *
		 * A call to QSettings::beginGroup() is not needed since it is done by the parent sheet already.
		 */
		virtual void stateSaveRestore(QSettings& settings, bool save);

	protected:
		/**
		 * @brief Connects all known controls change signals to a handler which enables or disables buttons.
		 *
		 * A derived class should call this method when the is setup and controls are updated.
		 * @return List of not connected control widgets.
		 */
		virtual QWidgetList connectControls();

		/**
		 * @brief Holds the sheet it is part of.
		 */
		PropertySheetDialog* _sheet;

		friend PropertySheetDialog;
};

}
