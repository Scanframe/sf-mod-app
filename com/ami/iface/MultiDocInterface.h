#pragma once

#include <QString>

namespace sf
{

/**
 * @brief Interface for widgets handling files.
 */
class MultiDocInterface
{
	public:
		/**
		 * @brief
		 */
		virtual void newFile() = 0;

		/**
		 * @brief
		 */
		virtual bool loadFile(const QString& fileName) = 0;

		/**
		 * @brief
		 */
		virtual bool save() = 0;

		/**
		 * @brief
		 */
		virtual bool saveAs() = 0;

		/**
		 * @brief
		 */
		virtual bool saveFile(const QString& fileName) = 0;

		/**
		 * @brief
		 */
		[[nodiscard]] virtual QString userFriendlyCurrentFile() const = 0;

		/**
		 * @brief
		 */
		[[nodiscard]] virtual QString currentFile() const = 0;

		/**
		 * @brief
		 */
		[[nodiscard]] virtual bool hasSelection() const = 0;

		/**
		 * @brief
		 */
		virtual void cut() = 0;

		/**
		 * @brief
		 */
		virtual void copy() = 0;

		/**
		 * @brief
		 */
		virtual void paste() = 0;

};

}
