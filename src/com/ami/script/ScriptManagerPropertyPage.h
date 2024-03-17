#include <misc/qt/PropertyPage.h>
#include "ScriptManager.h"

namespace Ui {class ScriptManagerPropertyPage;}

namespace sf
{

class ScriptManagerPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit ScriptManagerPropertyPage(ScriptManager* manager, PropertySheetDialog* parent = nullptr);

		~ScriptManagerPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		[[nodiscard]] bool isPageModified() const override;

		void applyPage() override;

	Q_SIGNALS:

		void openEditor(int index);

	private:
		ScriptManagerListModel* _listModel;
		Ui::ScriptManagerPropertyPage* ui;
		ScriptManager* _manager;

		QAction* _actionStartAll{nullptr};
		QAction* _actionStart{nullptr};
		QAction* _actionStop{nullptr};
		QAction* _actionEdit{nullptr};
		QAction* _actionAdd{nullptr};
		QAction* _actionRemove{nullptr};

};

}
