#include <misc/qt/PropertyPage.h>
#include "CodeEditorConfiguration.h"

namespace Ui {class CodeEditorPropertyPage;}

namespace sf
{

class CodeEditorPropertyPage :public PropertyPage
{
		Q_OBJECT

	public:
		explicit CodeEditorPropertyPage(CodeEditorConfiguration& cfg, PropertySheetDialog* parent = nullptr);

		~CodeEditorPropertyPage() override;

		[[nodiscard]] QIcon getPageIcon() const override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] bool isPageModified() const override;

		void updatePage() override;

		void applyPage() override;

		void afterPageApply() override;

	private:
		Ui::CodeEditorPropertyPage* ui;
		CodeEditorConfiguration& _configuration;
};

}
