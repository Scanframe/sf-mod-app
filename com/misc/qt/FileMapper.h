#pragma once

#include <QTemporaryFile>
#include "../gen/IFileMapper.h"
#include "../global.h"

namespace sf::qt
{

/**
 * @brief Qt implementation of the interface class #sf::IFileMapper.
 */
class _MISC_CLASS FileMapper :public IFileMapper
{
	public:
		/**
		 * Default constructor.
		 */
		FileMapper();

		/**
		 * Initializing constructor.
		 */
		FileMapper(const QString& filepath, qint64 ofs, qint64 sz);

/**
		 * Overridden from #sf::IFileMapper
 		 */
		void initialize() override;

		/**
		 * Initialize using an existing file.
		 * @param filepath
		 * @param ofs
		 * @param sz
		 */
		void initialize(const QString& filepath, qint64 ofs, qint64 sz);

		/**
		 * Sets the part of the file to be mapped.
		 */
		void setView(qint64 ofs, qint64 sz);

		/**
		 * Overridden from #sf::IFileMapper
 		 */
		~FileMapper() override;

		/**
		 * Overridden from #sf::IFileMapper
 		 */
		void createView(size_t sz) override;

		/**
		 * Overridden from #sf::IFileMapper
 		 */
		void* getPtr() override;
		/**
		 * Overridden from #sf::IFileMapper
 		 */
		bool mapView() override;
		/**
		 * Overridden from #sf::IFileMapper
 		 */
		bool unmapView() override;

		/**
		 * Returns the underlying file class.
		 */
		QFile& getFile();

	private:
		/**
		 * File being memory mapped.
		 */
		QTemporaryFile _file;
		/**
		 * Holds the locked pointer.
		 */
		uchar* _ptr{nullptr};
		/**
		 * Holds the mapped view size.
		 */
		qint64 _offset{0};
		/**
		 * Holds the mapped view offset.
		 */
		qint64 _size{0};
};

}
