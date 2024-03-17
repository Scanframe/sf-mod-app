#pragma once
#include <QDialog>
#include <qt/Namespace.h>
#include "../global.h"

namespace sf
{
/**
 * @brief Dialog showing the information on a single #sf::Variable or #sf::ResultData instance.
 *
 * Instance is automatically delete on close.
 */
class _GII_CLASS InformationMonitor :public QDialog
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 *
		 * @param parent Optional parent which defaults to the #sf::getGlobalParent()
		 */
		explicit InformationMonitor(QWidget* parent = nullptr);

		/**
		 * @brief Destructor.
		 */
		~InformationMonitor() override;

		/**
		 * @brief Sets the id to be monitored.
		 *
		 * @param typeId Type of the id Variable or Result-data type.
		 * @param id Information id.
		 */
		void setId(Gii::TypeId typeId, Gii::IdType id);

		/**
		 * @brief Opens a dialog to select the id to monitor.
		 *
		 * @param typeId Type of the id Variable or Result-data type.
		 * @param parent Optional parent which defaults to #sf::getGlobalParent().
		 * @return True when the dialog was accepted.
		 */
		bool selectId(Gii::TypeId typeId, QWidget* parent = nullptr);

	private:
		struct Private;
		Private* _p;
};

}
