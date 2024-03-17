#pragma once

#include <QtUiPlugin/QDesignerCustomWidgetInterface>

namespace sf
{

/**
 * @brief Interface class to create a #sf::VariableEdit custom widget.
 */
class AcquisitionControlPlugin :public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
		Q_INTERFACES(QDesignerCustomWidgetInterface)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit AcquisitionControlPlugin(QObject* parent = nullptr);

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] bool isContainer() const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] bool isInitialized() const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QIcon icon() const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QString domXml() const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QString group() const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QString includeFile() const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QString name() const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QString toolTip() const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QString whatsThis() const override;

		/**
		 * @brief Overridden from base class.
		 */
		QWidget* createWidget(QWidget* parent) override;

		/**
		 * @brief Overridden from base class.
		 */
		void initialize(QDesignerFormEditorInterface* core) override;

	private:
		bool initialized = false;
};

}
