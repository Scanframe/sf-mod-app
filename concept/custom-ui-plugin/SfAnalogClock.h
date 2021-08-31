#pragma once

#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>
#include <misc/qt/ObjectExtension.h>
#include "namespace.h"

namespace sf
{

class QDESIGNER_WIDGET_EXPORT AnalogClock :public QWidget, public ObjectExtension
{
	Q_OBJECT
		Q_PROPERTY(My::Global::MyEnum myGlobalEnum READ getMyGlobalEnum WRITE setMyGlobalEnum);
		Q_PROPERTY(MyEnum myEnum READ getMyEnum WRITE setMyEnum);
		// Use the full namespace path from root otherwise it is not visible in the designer.
		Q_PROPERTY(sf::Gii::IdType idType READ getIdType WRITE setIdType);

	public:
		enum MyEnum
		{
			Option1,
			Option2,
			Option3
		};
		Q_ENUM(MyEnum)

		void setMyGlobalEnum(My::Global::MyEnum value) {_myGlobalEnum = value;};
		[[nodiscard]] My::Global::MyEnum getMyGlobalEnum() const {return _myGlobalEnum;};

		void setMyEnum(MyEnum value) {_myEnum = value;};
		[[nodiscard]] MyEnum getMyEnum() const {return _myEnum;};

		void setIdType(Gii::IdType value) {_idType = value;};
		[[nodiscard]] Gii::IdType getIdType() const {return _idType;};

		explicit AnalogClock(QWidget* parent = nullptr);

	protected:
		bool isRequiredProperty(const QString& name) override;

		void paintEvent(QPaintEvent* event) override;

		My::Global::MyEnum _myGlobalEnum{My::Global::Option1};
		MyEnum _myEnum{Option1};
		Gii::IdType _idType{Gii::Variable};

};

} // namespace
