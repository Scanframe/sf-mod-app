#pragma once

#include <QDialog>
#include "FollowersListModel.h"

namespace sf
{

namespace Ui {class FollowersDialog;}

class FollowersDialog :public QDialog
{
	Q_OBJECT

	public:
		explicit FollowersDialog(QWidget* parent = nullptr);

		~FollowersDialog() override;

		bool execute(FollowersListModel::Entry& fle);

	private:
		Ui::FollowersDialog* ui;

		void okay();

		bool checkContent();

		QAction* _actionEdit{nullptr};
		QAction* _actionAdd{nullptr};
		QAction* _actionRemove{nullptr};
};

}
