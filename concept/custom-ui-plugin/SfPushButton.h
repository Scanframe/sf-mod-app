#pragma once

#include <QPushButton>
#include <QtUiPlugin/QDesignerExportWidget>
#include <misc/qt/ObjectExtension.h>
#include <misc/qt/Macros.h>

namespace sf
{

class QDESIGNER_WIDGET_EXPORT PushButton :public QPushButton, public ObjectExtension
{
	Q_OBJECT
		Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY changed)
		Q_PROPERTY(int intSome MEMBER _intSome NOTIFY changed)
		Q_PROPERTY(qreal floatSome READ getFloatSome WRITE setFloatSome)
		SF_DECL_PROP_GS(qreal,  FloatSome)

	public:
		explicit PushButton(QWidget* parent = nullptr);

		void changed();

	protected:
		void connectNotify(const QMetaMethod& signal) override;

	protected:
		bool isRequiredProperty(const QString& name) override;

		void mousePressEvent(QMouseEvent* event) override;

		void setIconSource(const QString& text);

		[[nodiscard]] QString iconSource() const;

		void changed(void*);

	private:
		qreal _floatSome{1.1};
		int _intSome{-1};
		QString _iconSource;
};

} // namespace
