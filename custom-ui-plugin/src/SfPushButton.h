#ifndef SFPUSHBUTTON_H
#define SFPUSHBUTTON_H

#include <QPushButton>
#include <QtUiPlugin/QDesignerExportWidget>
#include <misc/objectextension.h>

namespace sf
{

class QDESIGNER_WIDGET_EXPORT PushButton :public QPushButton, public ObjectExtension
{
	Q_OBJECT
		Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource)

	public:
		explicit PushButton(QWidget* parent = nullptr);

	protected:
		bool isRequiredProperty(const QString& name) override;

		void mousePressEvent(QMouseEvent* event) override;

		void setIconSource(const QString& text);

		QString iconSource() const;

	private:
		QString IconSource;
};

} // namespace

#endif
