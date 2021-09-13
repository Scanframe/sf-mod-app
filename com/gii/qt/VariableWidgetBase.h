#pragma once

#include <QObject>
#include <QFrame>
#include <QtUiPlugin/QDesignerExportWidget>
#include <misc/qt/ObjectExtension.h>

namespace sf
{

/**
 * Forward definition of private implemented class to prevent interfaces exposure.
 */
class Variable;

/**
 * @brief Base class for #sf::Variable based widgets.
 */
class QDESIGNER_WIDGET_EXPORT VariableWidgetBase :public QWidget, public ObjectExtension
{
	Q_OBJECT
		Q_PROPERTY(qulonglong idVariable READ getId WRITE setId)
		Q_PROPERTY(bool converted READ getConverted WRITE setConverted)
		Q_PROPERTY(bool readOnly READ getReadOnly WRITE setReadOnly)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit VariableWidgetBase(QWidget* parent, QObject* self);

		/**
		 * @brief Destructor.
		 */
		~VariableWidgetBase() override;

		/**
		 * @brief Property 'variableId' set method.
		 */
		void setId(qulonglong id);

		/**
		 * @brief Property 'variableId' get method.
		 */
		[[nodiscard]] qulonglong getId() const;

		/**
		 * @brief Gets the reference to the underlying Variable instance.
		 */
		[[nodiscard]] Variable& getVariable();

		/**
		 * @brief Property 'converted' set method.
		 */
		void setConverted(bool);

		/**
		 * @brief Property 'converted' get method.
		 */
		[[nodiscard]] bool getConverted() const;

		/**
		 * @brief Property 'converted' set method.
		 */
		void setReadOnly(bool);

		/**
		 * @brief Property 'converted' get method.
		 */
		[[nodiscard]] bool getReadOnly() const;

		/**
		 * @brief Overridden from base class 'ObjectExtension' to write only the properties that mean anything.
		 */
		bool isRequiredProperty(const QString& name) override;

		/**
		 * @brief Property name of the property which guides the layout editor to the proper parent.
		 *
		 * @return Property name.
		 */
		static const char* propertyNameEditorObject();

	protected:

		/**
		 * To handle a request for read only behavior of the widget.
		 * @param yn
		 */
		virtual void applyReadOnly(bool yn) = 0;

		struct PrivateBase;
		/**
		 * @brief Holds the private object or a derived one.
		 */
		PrivateBase* _p{nullptr};

	private:

		typedef QWidget base_type;
};

}
