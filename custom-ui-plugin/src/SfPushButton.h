#ifndef SFPUSHBUTTON_H
#define SFPUSHBUTTON_H

#include <QPushButton>
#include <QtUiPlugin/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT SfPushButton : public QPushButton
{
	Q_OBJECT

	public:
		explicit SfPushButton(QWidget *parent = nullptr);

	protected:
		void mousePressEvent(QMouseEvent* event) override;
};

#endif
