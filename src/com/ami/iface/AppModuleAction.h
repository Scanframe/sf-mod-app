#pragma once

#include <QObject>
#include <QAction>
#include <QKeySequence>
#include <QIcon>

#include "global.h"

namespace sf
{

// Forward definition.
class AppModuleActions;

/**
 * @brief Holds a single application module action.
 */
class _AMI_CLASS AppModuleAction :public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 *
		 * @param parent Owning parent.
		 */
		explicit AppModuleAction(AppModuleActions* parent);

		/**
		 * @brief Gets a new created action entry from this instance.
		 *
		 * @return New formed action.
		 */
		QAction getAction(QObject* parent) const;

		/**
		 * @brief Types of action
		 */
		enum EType
		{
			/**
			 * @brief Action
			 */
			atIndependent,
			/**
			 * @brief Action applies to a multiple document widget created by the module.
			 */
			atMultiDocument
		};

	private:
		/**
		 * @brief Name of the action.
		 */
		QString _name;
		/**
		 * @brief Description of the action.
		 */
		QString _description;
		/**
		 * @brief Icon for the action.
		 */
		QIcon _icon;
		/**
		 * @brief The key sequence to activate this action.
		 */
		QKeySequence _keySequence;
};

}
