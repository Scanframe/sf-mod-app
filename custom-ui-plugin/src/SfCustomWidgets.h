#ifndef SFCUSTOMWIDGETS_H
#define SFCUSTOMWIDGETS_H

#include <QtUiPlugin/QDesignerCustomWidgetInterface>

namespace sf
{

class CustomWidgets :public QObject, public QDesignerCustomWidgetCollectionInterface
{
	Q_OBJECT
		Q_PLUGIN_METADATA(IID QDesignerCustomWidgetCollectionInterface_iid)
		Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

	public:
		explicit CustomWidgets(QObject* parent = nullptr);

		QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

	private:
		QList<QDesignerCustomWidgetInterface*> widgets;
};

} // namespace

#endif
