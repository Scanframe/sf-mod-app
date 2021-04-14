#pragma once

#include <QAbstractListModel>
#include <QMenu>

#include <iostream>
#include "../gen/LineBuffer.h"
#include "../global.h"

namespace sf
{

/**
 * @brief List model implementation on captured output stream data.
 */
class _MISC_CLASS CaptureListModel :public QAbstractListModel
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit CaptureListModel(QObject* parent = nullptr);

		/**
		 * @brief Destructor.
		 */
		~CaptureListModel() override;

		/**
		 * @brief Append string to the end of the list.
		 */
		bool append(const QString& str);

		/**
		 * @brief Stream sources to capture.
		 */
		enum StreamSource :unsigned
		{
			/** Captures the std::cout stream.*/
			ssCout = 1u << 0u,
			/** Captures the std::clog stream.*/
			ssClog = 1u << 1u,
			/** Captures the std::cerr stream.*/
			ssCerr = 1u << 2u
		};

		/**
		 * Sets the stream to be captured.
		 */
		unsigned setSource(unsigned ss);

		/**
		 * @brief Reports the stream being captured.
		 */
		[[nodiscard]] unsigned source() const;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QVariant
		headerData(int section, Qt::Orientation orientation, int role/* = Qt::DisplayRole*/) const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] int rowCount(const QModelIndex& parent/* = QModelIndex()*/) const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

	private slots:

		/**
		 * @brief Menu handler for clearing the content.
		 */
		void onClear();

	private:
		// Holds the stream source
		unsigned _source{0};
		// Pointer to own stream buffer.
		LineBuffer* _streamBuf{nullptr};
		// Pointers to saved stream buffers of the standard streams.
		static std::streambuf* _coutSaved;
		static std::streambuf* _clogSaved;
		static std::streambuf* _cerrSaved;
		// Holds the context menu.
		QMenu* _contextMenu{nullptr};

		void handleNewLine(LineBuffer* sender, const QString& line);

		void contextMenuRequested(QPoint);
};

} // namespace
