#pragma once

#include "PaletteInterface.h"
#include "ColorTable.h"
#include <misc/qt/PropertySheetDialog.h>
#include <misc/qt/Macros.h>
#include <QPainter>

namespace sf
{

/**
 * @brief Server for color palettes used in images for coloring on basis of index.
 */
class _PAL_CLASS PaletteServer :public QObject
{
	Q_OBJECT

	public:

		/**
		 * @brief Constructor.
		 */
		explicit PaletteServer(QObject* parent = nullptr);

		/**
		 * @brief Destructor.
		 */
		~PaletteServer() override;

		/**
		 * @brief Adds property pages of the select palette.
		 */
		void addPropertyPages(PropertySheetDialog* sheet);

		/**
		 * @brief Gets the generated color table of the selected.
		 */
		[[nodiscard]] ColorTable getColorTable() const;

		/**
		 * Sets the amount of colors and the amount really used in the pallet.
		 * @param colorsUsed Amount of colors used.
		 * @param colorsSize Total amount of colors (value of -1) defaults to 256.
		 */
		void setColorCount(int colorsUsed, int colorsSize = -1);

		/**
		 * @brief Paints the palette using the passed painter instance.
		 */
		void paint(QPainter& painter, const QRect& bounds) const;

		/**
		 * @brief Gets the names of all available implementations names and descriptions in pairs.
		 */
		[[nodiscard]] QList<QPair<QString, QString>> getImplementationNames() const;

		/**
		 * @brief Gets the name of the desired implementation.
		 */
		[[nodiscard]] const QString& getImplementationName() const;

		/**
		 * @brief Returns true when an implementation is available.
		 */
		[[nodiscard]] bool isAvailable() const;

		/**
		 * @brief Sets the name of the desired implementation.
		 */
		void setImplementationName(const QString& name);
		/**
		 * @brief Gets the name of the desired implementation.
		 */
		[[nodiscard]] const QStringList& getImplementationProperties();

		/**
		 * @brief Sets the name of the desired implementation.
		 */
		void setImplementationProperties(const QStringList& props);

		/**
		 * @brief Signal for when the palette changes.
		 */
		Q_SIGNAL void changed(PaletteServer* sender);

	protected:
		/**
		 * @brief Creates implementation by registered name.
		 * @return True on success.
		 */
		bool createImplementation(const std::string& name);

	private:
		// Gets the registered name of the current implementation.
		[[nodiscard]] std::string currentImplementationName() const;
		// Holds the desired name of the implementation.
		QString _implementationName;
		// Holds the implementation of the palette.
		PaletteInterface* _implementation;
		QStringList _implementationProperties;
		// Holds the size of the colors needed.
		int _colorsSize;
		// Holds the amount of colors used for the calculating the scale.
		int _colorsUsed;
		// Local fast copy of the generated table.
		ColorTable _colorTable;
		// Holds the flag to generate the color table.
		bool _flagGenerate;

		friend class PaletteServerPropertyPage;
};

}
