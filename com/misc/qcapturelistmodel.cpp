#include <QDebug>
#include <QListView>

#include "qcapturelistmodel.h"
#include "genutils.h"
#include "qt_utils.h"
#include "qresource.h"

namespace sf
{

std::streambuf* QCaptureListModel::_coutSaved = nullptr;
std::streambuf* QCaptureListModel::_clogSaved = nullptr;
std::streambuf* QCaptureListModel::_cerrSaved = nullptr;

QCaptureListModel::QCaptureListModel(QObject* parent)
	: QAbstractListModel(parent)
	, _source(0)
	, _streamBuf(new LineBuffer(300))
	, _contextMenu(nullptr)
{
	// Assign the new line event handler.
	_streamBuf->setNewLineHandler(LineBuffer::event_t([this](LineBuffer* sender, const std::string& line)->void
	{
		append(line.c_str());
	}));
	// When the parent is a widget.
	if (auto avi = dynamic_cast<QAbstractItemView*>(parent))
	{
		avi->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(avi, &QWidget::customContextMenuRequested, this, &QCaptureListModel::contextMenuRequested);
		_contextMenu = new QMenu(avi);
		//
		auto action = new QAction(Resource::getIcon(Resource::Icon::Clear), "Clear", this);
		_contextMenu->addAction(action);
		connect(action, &QAction::triggered, this, &QCaptureListModel::onClear);

#ifdef _FILL_ACTION
		action = new QAction(QIcon(":action/submit"), "Fill", this);
		_contextMenu->addAction(action);
		connect(action, &QAction::triggered, this, &QCaptureListModel::onFill);
#endif
	}
}

void QCaptureListModel::onClear()
{
	beginRemoveColumns(QModelIndex(), 0, _streamBuf->lineCount());
	_streamBuf->clear();
	endRemoveColumns();

}

void QCaptureListModel::onFill()
{
	static int idx = 0;
	for (int i = 0; i < 6; ++i)
	{
		std::clog << sf::string_format("This is line %03d", idx++) << std::endl;
	}
}

void QCaptureListModel::contextMenuRequested(QPoint pos)
{
	// When the parent is a listView.
	if (auto aiv = dynamic_cast<QAbstractItemView*>(parent()))
	{
		//QModelIndex index = aiv->indexAt(pos);
		_contextMenu->popup(aiv->viewport()->mapToGlobal(pos));
	}
}

void QCaptureListModel::handleNewLine(LineBuffer* sender, const QString& line)
{
	append(line);
}

QCaptureListModel::~QCaptureListModel()
{
	if (_coutSaved)
	{
		std::cout.rdbuf(_coutSaved);
	}
	if (_clogSaved)
	{
		std::clog.rdbuf(_clogSaved);
	}
	if (_cerrSaved)
	{
		std::cerr.rdbuf(_cerrSaved);
	}
	delete _streamBuf;
}

int QCaptureListModel::rowCount(const QModelIndex& parent) const
{
	return _streamBuf->lineCount();
}

QVariant QCaptureListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	if (index.row() >= _streamBuf->lineCount())
	{
		return QVariant();
	}
	if (role == Qt::DisplayRole)
	{
		return QString::fromStdString(_streamBuf->getLine(index.row()));
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
	if (_streamBuf->lineCount())
	{
		dataChanged(QModelIndex(), index(_streamBuf->lineCount() - 1));
	}
/*
	beginInsertRows(QModelIndex(), _streamBuf->lineCount() - 1, _streamBuf->lineCount());
	endInsertRows();
*/

	// Automatically scroll to bottom.
	if (auto lv = dynamic_cast<QListView*>(parent()))
	{
		lv->scrollToBottom();
	}
	return true;
}

unsigned QCaptureListModel::source() const
{
	return _source;
}

unsigned QCaptureListModel::setSource(unsigned ss)
{
	_source = ss;
	if (_source & ssCout)
	{
		if (!_coutSaved)
		{
			_coutSaved = std::cout.rdbuf();
		}
		std::cout.rdbuf(_streamBuf);
	}
	if (_source & ssClog)
	{
		if (!_clogSaved)
		{
			_clogSaved = std::clog.rdbuf();
		}
		std::clog.rdbuf(_streamBuf);
	}
	if (_source & ssCerr)
	{
		if (!_cerrSaved)
		{
			_cerrSaved = std::cerr.rdbuf();
		}
		std::cerr.rdbuf(_streamBuf);
	}
	return _source;
}

} // namespace sf
