#pragma once
#include <QDialog>
#include <QListWidgetItem>
#include <misc/global.h>
#include <misc/qt/PropertyPage.h>

namespace sf
{

/**
 * @brief Dialog containing multiple property pages.
 */
class _MISC_CLASS PropertySheetDialog final : public QDialog
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructor.
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
		 * This method cannot be called from a property page's constructor.
		 * @param page
		 */
		void addPage(PropertyPage* page);

		/**
		 * @brief Gets the modified aggregated status of all property pages.
		 * @return True when the sheet is modified.
		 */
		[[nodiscard]] bool isSheetModified() const;

		/**
		 * @brief Applies changes of all pages in the sheet.
		 */
		void applySheet();

		int exec() override;

	protected:
		void showEvent(QShowEvent* event) override;

	public:
		/**
		 * @brief Emitted when a modification was applied.
		 */
		// ReSharper disable once CppFunctionIsNotImplemented
		Q_SIGNAL void modified();

		/**
		 * @brief Slot called from page when a control widget has been changed.
		 */
		Q_SLOT void checkModified(QWidget* origin);

	private:
		/**
		 * @brief Private class to hide needed members.
		 */
		struct Private;
		Private* _p;
};

}// namespace sf
