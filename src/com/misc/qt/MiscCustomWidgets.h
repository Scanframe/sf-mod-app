#pragma once

#include <QtUiPlugin/QDesignerCustomWidgetInterface>

namespace sf
{

/**
 * @brief Implementation of interface class for the creation of Misc custom widgets.
 */
class MiscCustomWidgets :public QObject, public QDesignerCustomWidgetCollectionInterface
{
	Q_OBJECT
		Q_PLUGIN_METADATA(IID QDesignerCustomWidgetCollectionInterface_iid)
		Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit MiscCustomWidgets(QObject* parent = nullptr);

		/**
		 * @brief Overridden destructor.
		 */
		~MiscCustomWidgets() override;

		/**
		 * @brief Overridden to return our own widget plugin classes.
		 */
		[[nodiscard]] QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

	private:
		/**
		 * @brief Holds the list of created plugin classes in the constructor.
		 */
		QList<QDesignerCustomWidgetInterface*> widgets;
};

}
