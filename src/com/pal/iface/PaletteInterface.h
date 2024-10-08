#pragma once

#include "ColorTable.h"
#include "Persistent.h"
#include "global.h"
#include <misc/gen/TClassRegistration.h>
#include <misc/gen/TClosure.h>
#include <misc/gen/Value.h>
#include <utility>

namespace sf
{

// Forward definition.
class PropertySheetDialog;

/**
 * @brief Base class for the interface.
 */
class _PAL_CLASS PaletteInterface : public QObject
	, public Persistent
{
		Q_OBJECT

	public:
		struct Parameters
		{
				typedef TClosure<void> Callback;

				explicit Parameters(const Callback& callback, QObject* parent = nullptr)
					: _parent(parent)
					, _callback(callback)
				{
				}

				QObject* _parent{};
				Callback _callback;
		};

		/**
		 * @brief Constructor for passing general structure for derived classes.
		 */
		explicit PaletteInterface(const Parameters&);

		/**
		 * @brief Virtual destructor for derived classes.
		 */
		~PaletteInterface() override = default;

		/**
		 * Adds controller specific property pages to the passed sheet.
		 * @param sheet
		 */
		virtual void addPropertyPages(PropertySheetDialog* sheet);

		/**
		 * @brief Sets the size of the color table of the implementation.
		 *
		 * @param colorsUsed Amount of colors actually used.
		 * @param colorsSize  Amount of colors needed.
		 */
		virtual void setSizes(qsizetype colorsUsed, qsizetype colorsSize);

		/**
		 * @brief Gets the color table of the implementation.
		 */
		[[nodiscard]] virtual ColorTable getColorTable() const = 0;

		/**
		 * @brief Gets calculated level for color shifts.
		 *
		 * Used in calculation of colors and drawing a curve when configuring.
		 * @param x Value between 0.0 and 1.0.
		 * @return Value between 0.0 and 1.0.
		 *
		 */
		[[nodiscard]] virtual qreal getCurveLevel(qreal x) const = 0;

	protected:
		Parameters::Callback _callback;
		qsizetype _colorsUsed{256};
		qsizetype _colorsSize{256};

		// Declarations of static functions and data members to be able to create registered implementations.
		SF_DECL_IFACE(PaletteInterface, PaletteInterface::Parameters, Interface)
};

}// namespace sf
