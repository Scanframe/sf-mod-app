#pragma once

#include <QPushButton>
#include <QtUiPlugin/QDesignerExportWidget>
#include <misc/qt/ObjectExtension.h>

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

		[[nodiscard]] QString iconSource() const;

	private:
		QString _iconSource;
};

} // namespace
