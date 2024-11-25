#pragma once
#include <QAction>
#include <QPushButton>
#include <QtUiPlugin/QDesignerExportWidget>
#include <misc/qt/ObjectExtension.h>

namespace sf
{

/**
 * @brief A QPushButton which is associated with an action.
 */
class QDESIGNER_WIDGET_EXPORT ActionButton
	: public QPushButton
	, public ObjectExtension
{
		Q_OBJECT
		Q_PROPERTY(QString action READ getActionByName WRITE setActionByName)

	public:
		/**
		 * @brief Standard widget constructor.
		 */
		explicit ActionButton(QWidget* parent = nullptr);

		/**
		 * @brief Overloaded from base class.
		 */
		bool isRequiredProperty(const QString& name) override;

		/**
		 * @brief Set the action owner of this button, that is the action associated to the button.
		 * The button is configured immediately depending on the action status and the button and the action
		 * are connected together so that when the action is changed the button
		 * is updated and when the button has been clicked the 'action' is triggered.
		 * @param action The action to associate to this button.
		 */
		void setAction(QAction* action);

		/**
		 * @brief Gets the current associated action.
		 * @return Could be nullptr.
		 */
		[[nodiscard]] QAction* getAction() const;

		/**
		 * @brief Sets an 'action' by finding it in its parent child list by name.
		 * @param name Name of the action to look for.
		 */
		void setActionByName(const QString& name);

		/**
		 * @brief Property handler function to retrieve the actions name.
		 * @return Name of the action.
		 */
		[[nodiscard]] QString getActionByName() const;

	public Q_SLOTS:

		void connectAction(QAction* action);

		/**
		 * @brief Update the button status depending on a change on the action status.
		 * This slot is invoked each time the action "changed" signal is emitted.
		 */
		void updateButtonStatusFromAction();

	private:
		/**
		 * @brief Holds the associated action.
		 */
		QAction* _action;
		/**
		 * @brief Holds the associated action name.
		 */
		QString _actionName;
};

}// namespace sf
