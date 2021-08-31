#pragma once

#include <QtUiPlugin/QDesignerExportWidget>
#include <QWidget>
#include <QPaintEvent>
#include <misc/qt/ObjectExtension.h>

namespace sf
{

class QDESIGNER_WIDGET_EXPORT AscanGraph :public QWidget, public ObjectExtension
{
	Q_OBJECT

	public:
		explicit AscanGraph(QWidget* parent = nullptr);

		void paintEvent(QPaintEvent* event) override;

		QSize minimumSizeHint() const override;

		bool isRequiredProperty(const QString& name) override;

	private:
		struct Private;
		Private* _p;
};

}
