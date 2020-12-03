#include <QDebug>
#include <QtWidgets/QListView>

#include "qcapturelistmodel.h"

namespace sf
{

std::streambuf* QCaptureListModel::FCOutSaved = nullptr;
std::streambuf* QCaptureListModel::FCLogSaved = nullptr;
std::streambuf* QCaptureListModel::FCErrSaved = nullptr;

QCaptureListModel::QCaptureListModel(QObject* parent)
	: QAbstractListModel(parent)
	, FStringList()
	, FSource(0)
	, FStreamBuf(new LineBuffer(10))
{
	// Assign the new line event handler.
	FStreamBuf->setNewLineHandler(LineBuffer::EvNewLine([this](LineBuffer* sender, const std::string& line)->void
	{
		append(line.c_str());
	}));
}

void QCaptureListModel::HandleNewLine(LineBuffer* sender, const QString& line)
{
	append(line);
}

QCaptureListModel::~QCaptureListModel()
{
	if (FCOutSaved)
	{
		std::cout.rdbuf(FCOutSaved);
	}
	if (FCLogSaved)
	{
		std::clog.rdbuf(FCLogSaved);
	}
	if (FCErrSaved)
	{
		std::cerr.rdbuf(FCErrSaved);
	}
	delete FStreamBuf;
}

int QCaptureListModel::rowCount(const QModelIndex& parent) const
{
	return FStringList.count();
}

QVariant QCaptureListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	if (index.row() >= FStringList.size())
	{
		return QVariant();
	}
	if (role == Qt::DisplayRole)
	{
		return FStringList.at(index.row());
	}
	else
	{
		return QVariant();
	}
}

QVariant QCaptureListModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	if (orientation == Qt::Horizontal)
	{
		return QString("Column %1").arg(section);
	}
	else
	{
		return QString("Row %1").arg(section);
	}
}

bool QCaptureListModel::append(const QString& str)
{
	beginInsertRows(QModelIndex(), FStringList.size(), FStringList.size() + 1);
	FStringList.append(str);
	endInsertRows();
	// Automatically scroll to bottom.
	if (auto lv = dynamic_cast<QListView*>(parent()))
	{
		lv->scrollToBottom();
	}
	return true;
}

unsigned QCaptureListModel::source() const
{
	return FSource;
}

unsigned QCaptureListModel::setSource(unsigned ss)
{
	FSource = ss;
	if (FSource & ssCout)
	{
		if (!FCOutSaved)
		{
			FCOutSaved = std::cout.rdbuf();
		}
		std::cout.rdbuf(FStreamBuf);
	}
	if (FSource & ssClog)
	{
		if (!FCLogSaved)
		{
			FCLogSaved = std::clog.rdbuf();
		}
		std::clog.rdbuf(FStreamBuf);
	}
	if (FSource & ssCerr)
	{
		if (!FCErrSaved)
		{
			FCErrSaved = std::cerr.rdbuf();
		}
		std::cerr.rdbuf(FStreamBuf);
	}
	return FSource;
}

} // namespace sf
