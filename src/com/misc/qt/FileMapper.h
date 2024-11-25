#pragma once
#include <QTemporaryFile>
#include <misc/gen/IFileMapper.h>
#include <misc/global.h>

namespace sf::qt
{

/**
 * @brief Qt implementation of the interface class #sf::IFileMapper.
 */
class _MISC_CLASS FileMapper : public IFileMapper
{
	public:
		/**
		 * @brief Default constructor.
		 */
		FileMapper();

		/**
		 * @brief Initializing constructor.
		 * @param filepath Path to the file.
		 * @param ofs Offset into the file.
		 * @param sz Size of the chunk in the file.
		 */
		FileMapper(const QString& filepath, qint64 ofs, qint64 sz);

		/**
		 * @brief Overridden from #sf::IFileMapper.
 		 */
		void initialize() override;

		/**
		 * @brief Initialize using an existing file.
		 * @param filepath Path to the file.
		 * @param ofs Offset into the file.
		 * @param sz Size of the chunk in the file.
		 */
		void initialize(const QString& filepath, qint64 ofs, qint64 sz);

		/**
		 * @brief Sets the part of the file to be mapped.
		 * @param ofs Offset into the file.
		 * @param sz Size of the chunk in the file.
		 */
		void setView(qint64 ofs, qint64 sz);

		/**
		 * @brief Overridden from #sf::IFileMapper.
 		 */
		~FileMapper() override;

		/**
		 * @brief Overridden from #sf::IFileMapper.
 		 */
		void createView(size_t sz) override;

		/**
		 * @brief Overridden from #sf::IFileMapper.
 		 */
		void* getPtr() override;
		/**
		 * @brief Overridden from #sf::IFileMapper.
 		 */
		bool mapView() override;
		/**
		 * @brief Overridden from #sf::IFileMapper.
 		 */
		bool unmapView() override;

		/**
		 * @brief Gets the underlying file class.
		 */
		QFile& getFile();

	private:
		/**
		 * @brief File being memory mapped.
		 */
		QTemporaryFile _file;
		/**
		 * @brief Holds the locked pointer.
		 */
		uchar* _ptr{nullptr};
		/**
		 * @brief Holds the mapped view offset.
		 */
		qint64 _offset{0};
		/**
		 * @brief Holds the mapped view size.
		 */
		qint64 _size{0};
};

}// namespace sf::qt
