#include <QDebug>
#include <QListView>
#include <QApplication>

#include "CaptureListModel.h"
#include "gen/gen_utils.h"
#include "qt/qt_utils.h"
#include "Resource.h"

namespace sf
{

std::streambuf* CaptureListModel::_coutSaved = nullptr;
std::streambuf* CaptureListModel::_clogSaved = nullptr;
std::streambuf* CaptureListModel::_cerrSaved = nullptr;

CaptureListModel::CaptureListModel(QObject* parent)
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
	if (auto avi = qobject_cast<QAbstractItemView*>(parent))
	{
		avi->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(avi, &QWidget::customContextMenuRequested, this, &CaptureListModel::contextMenuRequested);
		_contextMenu = new QMenu(avi);
		//
		auto action = new QAction(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Clear), QApplication::palette(), QPalette::ColorRole::ButtonText), "Clear", this);
		_contextMenu->addAction(action);
		connect(action, &QAction::triggered, this, &CaptureListModel::onClear);

#ifdef _FILL_ACTION
		action = new QAction(QIcon(":action/submit"), "Fill", this);
		_contextMenu->addAction(action);
		connect(action, &QAction::triggered, this, &CaptureListModel::onFill);
#endif
	}
}

void CaptureListModel::onClear()
{
	beginRemoveColumns(QModelIndex(), 0, _streamBuf->lineCount());
	_streamBuf->clear();
	endRemoveColumns();
}

void CaptureListModel::contextMenuRequested(QPoint pos)
{
	// When the parent is a listView.
	if (auto aiv = qobject_cast<QAbstractItemView*>(parent()))
	{
		//QModelIndex index = aiv->indexAt(pos);
		_contextMenu->popup(aiv->viewport()->mapToGlobal(pos));
	}
}

void CaptureListModel::handleNewLine(LineBuffer* sender, const QString& line)
{
	append(line);
}

CaptureListModel::~CaptureListModel()
{
	// Restore initial status.
	setSource(0);
	delete _streamBuf;
}

int CaptureListModel::rowCount(const QModelIndex& parent) const
{
	return _streamBuf->lineCount();
}

QVariant CaptureListModel::data(const QModelIndex& index, int role) const
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

QVariant CaptureListModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool CaptureListModel::append(const QString& str)
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
	if (auto lv = qobject_cast<QListView*>(parent()))
	{
		lv->scrollToBottom();
	}
	return true;
}

unsigned CaptureListModel::source() const
{
	return _source;
}

unsigned CaptureListModel::setSource(unsigned ss)
{
	// Only perform when there is a change.
	if (ss == _source)
	{
		return _source;
	}
	// Assign the value.
	_source = ss;
	//
	if (_source & ssCout)
	{
		if (!_coutSaved)
		{
			_coutSaved = std::cout.rdbuf();
		}
		std::cout.rdbuf(_streamBuf);
	}
	else
	{
		if (_coutSaved)
		{
			std::cout.rdbuf(_coutSaved);
		}
	}
	//
	if (_source & ssClog)
	{
		if (!_clogSaved)
		{
			_clogSaved = std::clog.rdbuf();
		}
		std::clog.rdbuf(_streamBuf);
	}
	else
	{
		if (_clogSaved)
		{
			std::clog.rdbuf(_clogSaved);
		}
	}
	//
	if (_source & ssCerr)
	{
		if (!_cerrSaved)
		{
			_cerrSaved = std::cerr.rdbuf();
		}
		std::cerr.rdbuf(_streamBuf);
	}
	else
	{
		if (_cerrSaved)
		{
			std::cerr.rdbuf(_cerrSaved);
		}
	}
	//
	return _source;
}

}
