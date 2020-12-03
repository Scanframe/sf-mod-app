#ifndef STRINGLISTMODEL_H
#define STRINGLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>

class StringListModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		explicit StringListModel(QObject *parent = nullptr);

		int rowCount(const QModelIndex &parent/* = QModelIndex()*/) const override;

		QVariant data(const QModelIndex &index, int role) const override;

		QVariant headerData(int section, Qt::Orientation orientation, int role/* = Qt::DisplayRole*/) const override;

		void append(const QString& str);

	private:
		QStringList stringList;
};

#endif // STRINGLISTMODEL_H