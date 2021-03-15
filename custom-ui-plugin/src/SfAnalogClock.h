#pragma once

#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>
#include <misc/qt/ObjectExtension.h>

namespace sf
{

class QDESIGNER_WIDGET_EXPORT AnalogClock :public QWidget, public ObjectExtension
{
	Q_OBJECT

	public:
		explicit AnalogClock(QWidget* parent = nullptr);

	protected:
		bool isRequiredProperty(const QString& name) override;

		void paintEvent(QPaintEvent* event) override;
};

} // namespace
