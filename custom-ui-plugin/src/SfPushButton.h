#ifndef SFPUSHBUTTON_H
#define SFPUSHBUTTON_H

#include <QPushButton>
#include <QtUiPlugin/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT SfPushButton :public QPushButton
{
	Q_OBJECT
		Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource)

	public:
		explicit SfPushButton(QWidget* parent = nullptr);

	protected:
		void mousePressEvent(QMouseEvent* event) override;

		void setIconSource(const QString &text);

		QString iconSource() const;

	private:
		QString IconSource;
};

#endif
