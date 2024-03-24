#pragma once

#include <qt/VariableWidgetBase.h>
#include <misc/qt/Macros.h>
#include <QTabWidget>

namespace sf
{

/**
 * @brief Widget for editing #sf::Variable value types integer, floating point and single line string.
 */
class QDESIGNER_WIDGET_EXPORT LayoutTabs :public VariableWidgetBase
{
	Q_OBJECT
		Q_PROPERTY(QStringList tabsConfig READ getTabsConfig WRITE setTabsConfig)
		Q_PROPERTY(QTabWidget::TabPosition tabPosition READ getTabPosition WRITE setTabPosition)
		Q_PROPERTY(QTabWidget::TabShape tabShape READ getTabShape WRITE setTabShape)
		Q_PROPERTY(int currentIndex READ getCurrentIndex WRITE setCurrentIndex)

		// Propagate the 'TabPosition' property.
		SF_DECL_PROP_GS(QTabWidget::TabPosition, TabPosition)
		SF_DECL_PROP_GS(QTabWidget::TabShape, TabShape)
		SF_DECL_PROP_GS(int, CurrentIndex)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit LayoutTabs(QWidget* parent = nullptr);

		/**
		 * @brief Gets the tabs configuration where each string in the list configures a tab.
		 *
		 * The string is separated by a '|' character and has 3 fields.
		 * In the form of "name|id-offset|relative-filename".
		 * The file name does not have a suffix.
		 */
		[[nodiscard]] QStringList getTabsConfig() const;

		/**
		 * @brief Sets the tabs configuration where each string in the list configures a tab.
		 */
		void setTabsConfig(const QStringList& sl);

	protected:
		/**
		 * @brief Overridden from base class '#ObjectExtension'.
		 */
		bool isRequiredProperty(const QString& name) override;

		/**
		 * @brief Overridden from base class '#VariableWidgetBase'.
		 */
		void applyReadOnly(bool yn) override;

	private:

	/**
	 * @brief Forward definition of private implemented class to prevent interfaces exposure.
	 */
		struct Private;

		Q_DISABLE_COPY(LayoutTabs)
};

}
