#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>

namespace sf
{

class QDESIGNER_WIDGET_EXPORT AnalogClock :public QWidget
{
	Q_OBJECT

	public:
		explicit AnalogClock(QWidget* parent = nullptr);

	protected:
		void paintEvent(QPaintEvent* event) override;
};

} // namespace

#endif
