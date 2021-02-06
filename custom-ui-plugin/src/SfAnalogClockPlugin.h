#ifndef SFANALOGCLOCKPLUGIN_H
#define SFANALOGCLOCKPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetInterface>

namespace sf
{

class AnalogClockPlugin :public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
		Q_INTERFACES(QDesignerCustomWidgetInterface)

	public:
		explicit AnalogClockPlugin(QObject* parent = nullptr);

		bool isContainer() const override;

		bool isInitialized() const override;

		QIcon icon() const override;

		QString domXml() const override;

		QString group() const override;

		QString includeFile() const override;

		QString name() const override;

		QString toolTip() const override;

		QString whatsThis() const override;

		QWidget* createWidget(QWidget* parent) override;

		void initialize(QDesignerFormEditorInterface* core) override;

	private:
		bool initialized = false;
};

} // namespace

#endif
