#include "PropertyPage.h"
#include "Editor.h"
#include "../global.h"

namespace Ui {class EditorPropertyPage;}

namespace sf
{

class _MISC_CLASS EditorPropertyPage :public PropertyPage
{
		Q_OBJECT

	public:
		explicit EditorPropertyPage(Editor::Configuration& cfg, PropertySheetDialog* parent = nullptr);

		~EditorPropertyPage() override;

		[[nodiscard]] QIcon getPageIcon() const override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] bool isPageModified() const override;

		void updatePage() override;

		void applyPage() override;

		void afterPageApply(bool was_modified) override;

	private:
		Ui::EditorPropertyPage* ui;
		Editor::Configuration& _configuration;
};

}
