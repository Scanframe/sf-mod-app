#pragma once

#include "ObjectExtension.h"
#include <QAction>
#include <QPushButton>
#include <QtUiPlugin/QDesignerExportWidget>

namespace sf
{

class QDESIGNER_WIDGET_EXPORT ActionButton
	: public QPushButton
	, public ObjectExtension
{
		Q_OBJECT
		Q_PROPERTY(QString action READ getActionByName WRITE setActionByName)

	public:
		/**
		 * @brief Constructor.
		 * @param parent
		 */
		explicit ActionButton(QWidget* parent = nullptr);

		bool isRequiredProperty(const QString& name) override;

		/**
		 * @brief Set the action owner of this button, that is the action associated to the button.
		 *
		 * The button is configured immediately depending on the action status and the button and the action
		 * are connected together so that when the action is changed the button
		 * is updated and when the button is clicked the action is triggered.
		 * action the action to associate to this button
		 */
		void setAction(QAction* action);

		[[nodiscard]] QAction* getAction() const;

		void setActionByName(const QString& name);

		[[nodiscard]] QString getActionByName() const;

	public Q_SLOTS:

		void connectAction(QAction* action);

		/**
		 * @brief Update the button status depending on a change on the action status.
		 *
		 * This slot is invoked each time the action "changed" signal is emitted.
		 */
		void updateButtonStatusFromAction();

	private:
		QAction* _action;
		QString _actionName;
};

}// namespace sf
