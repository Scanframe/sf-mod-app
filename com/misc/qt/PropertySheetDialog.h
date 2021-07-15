#pragma once
#include <QDialog>
#include <QMenu>
#include <QListView>
#include <QSettings>
#include <QListWidgetItem>
#include "PropertyPage.h"
#include "../global.h"

namespace sf
{

// Forward definition
class PropertyPage;

/**
 * @brief Dialog containing multiple property pages.
 */
class _MISC_CLASS PropertySheetDialog :public QDialog
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 *
		 * @param name Name for identifying this sheet for referencing.
		 * @param settings Settings to store
		 * @param parent Optional parent widget.
		 */
		explicit PropertySheetDialog(const QString& name, QSettings* settings, QWidget* parent = nullptr);

		/**
		 * @brief Destructor.
		 */
		~PropertySheetDialog() override;

		/**
		 * @brief Adds a property page to this sheet.
		 *
		 * This method cannot be called from a property page's constructor.
		 * @param page
		 */
		void addPage(PropertyPage* page);

		/**
		 * @brief Gets the modified aggregated status of all property pages.
		 *
		 * @return True when the sheet is modified.
		 */
		[[nodiscard]] bool isSheetModified() const;

		/**
		 * @brief Applies changes of all pages in the sheet.
		 */
		void applySheet();

		/**
		 * @brief Overridden from base class to change the button enable status.
		 */
		void open() override;

		/**
		 * @brief Overridden from base class to change the button enable status.
		 */
		int exec() override;

	public Q_SLOTS:
		/**
		 * @brief Slot called from page when a control widget has been changed.
		 */
		void checkModified(QWidget* origin);

	private:
		/**
		 * @brief Private class to hide needed members.
		 */
		struct Private;
		Private* _p;
};

}
