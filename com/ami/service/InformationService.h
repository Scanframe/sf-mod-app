#pragma once

#include <QObject>
#include <qt/Namespace.h>

namespace sf
{

/**
 * @brief Implements a connection between to application exporting and importing information.
 */
class InformationService :public QObject
{
	public:
		explicit InformationService(QObject* parent = nullptr);

		/**
		 * @brief Type determining the function of the service.
		 */
		enum EServiceType
		{
			/** No client or server connection. */
			None = 0,
			/** Client only. */
			Client,
			/** Server only. */
			Server,
			/** Both in the same application for debugging purposes. */
			Both
		};

		/**
		 * @brief Sets the type of service for this instance.
		 */
		void setType(EServiceType type);

		/**
		 * @brief gets the type of service of this instance.
		 */
		[[nodiscard]] EServiceType getType() const;

		/**
		 * @brief Sets the client idOffset for the information instances.
		 */
		void setIdOffset(Gii::IdType idOffset);

		/**
		 * @brief Gets the client idOffset of the information instances.
		 */
		[[nodiscard]] Gii::IdType getIdOffset() const;

	private:
		struct Private;
		Private* _p;
};

}
