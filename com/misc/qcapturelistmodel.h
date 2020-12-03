#ifndef MISC_QCAPTURELISTMODEL_H
#define MISC_QCAPTURELISTMODEL_H

#include <QAbstractListModel>
// Include for cout, cerr ,clog and ostream.
#include <iostream>

#include "linebuffer.h"

#include "global.h"

namespace sf
{

class _MISC_CLASS QCaptureListModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		/**
		 * Constructor.
		 */
		explicit QCaptureListModel(QObject* parent = nullptr);
		/**
		 * Destructor.
		 */
		~QCaptureListModel() override;

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
		unsigned source() const;
		//
		// Overloaded from base class.
		//
		QVariant headerData(int section, Qt::Orientation orientation, int role/* = Qt::DisplayRole*/) const override;
		int rowCount(const QModelIndex& parent/* = QModelIndex()*/) const override;
		QVariant data(const QModelIndex& index, int role) const override;

	private:
		// Holds the string list.
		QStringList FStringList;
		// Holds the stream source
		unsigned FSource;
		// Pointer to own stream buffer.
		LineBuffer* FStreamBuf;
		// Pointers to save stream buffers of the standard streams.
		static std::streambuf* FCOutSaved;
		static std::streambuf* FCLogSaved;
		static std::streambuf* FCErrSaved;
		//
		void HandleNewLine(LineBuffer* sender, const QString& line);
};

} // namespace sf

#endif // MISC_QCAPTURELISTMODEL_H