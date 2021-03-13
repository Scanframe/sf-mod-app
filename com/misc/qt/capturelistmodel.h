#pragma once

#include <QAbstractListModel>
#include <QMenu>

#include <iostream>
#include "../gen/LineBuffer.h"
#include "../global.h"

namespace sf
{

class _MISC_CLASS CaptureListModel :public QAbstractListModel
{
	Q_OBJECT

	public:
		/**
		 * Constructor.
		 */
		explicit CaptureListModel(QObject* parent = nullptr);
		/**
		 * Destructor.
		 */
		~CaptureListModel() override;

		/**
		 * Append string to the end of the list.
		 */
		bool append(const QString& str);

		/**
		 * Stream sources.
		 */
		enum StreamSource
		{
			ssCout = 1u << 0u, // Captures the cout stream.
			ssClog = 1u << 1u, // Captures the clog stream.
			ssCerr = 1u << 2u  // Captures the cerr stream.
		};
		/**
		 * Sets the stream to be captured.
		 */
		unsigned setSource(unsigned ss);
		/**
		 * Reports the stream being captured.
		 */
		[[nodiscard]] unsigned source() const;
		//
		// Overloaded from base class.
		//
		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role/* = Qt::DisplayRole*/) const override;
		[[nodiscard]] int rowCount(const QModelIndex& parent/* = QModelIndex()*/) const override;
		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

	private slots:
		void onClear();
		static void onFill();

	private:
		// Holds the stream source
		unsigned _source;
		// Pointer to own stream buffer.
		LineBuffer* _streamBuf;
		// Pointers to save stream buffers of the standard streams.
		static std::streambuf* _coutSaved;
		static std::streambuf* _clogSaved;
		static std::streambuf* _cerrSaved;
		// Holds the context menu.
		QMenu* _contextMenu;
		//
		void handleNewLine(LineBuffer* sender, const QString& line);
		void contextMenuRequested(QPoint);
};

} // namespace