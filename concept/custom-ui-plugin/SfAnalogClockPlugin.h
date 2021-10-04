#pragma once

#include <QtUiPlugin/QDesignerCustomWidgetInterface>

namespace sf
{

class AnalogClockPlugin :public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
		Q_INTERFACES(QDesignerCustomWidgetInterface)

	public:
		explicit AnalogClockPlugin(QObject* parent = nullptr);

		void initialize(QDesignerFormEditorInterface* core) override;

		[[nodiscard]] bool isContainer() const override;

		[[nodiscard]] bool isInitialized() const override;

		[[nodiscard]] QIcon icon() const override;

		[[nodiscard]] QString domXml() const override;

		[[nodiscard]] QString group() const override;

		[[nodiscard]] QString includeFile() const override;

		[[nodiscard]] QString name() const override;

		[[nodiscard]] QString toolTip() const override;

		[[nodiscard]] QString whatsThis() const override;

		QWidget* createWidget(QWidget* parent) override;

	private:
		bool initialized = false;
};

} // namespace
