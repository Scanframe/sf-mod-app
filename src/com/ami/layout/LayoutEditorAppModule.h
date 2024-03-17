#pragma once

#include <QTreeView>
#include <ami/iface/AppModuleInterface.h>
#include <ami/iface/MultiDocInterface.h>
#include "HierarchyViewer.h"

namespace sf
{

class [[maybe_unused]] LayoutEditorAppModule :public AppModuleInterface
{
	public:

		explicit LayoutEditorAppModule(const Parameters& params);

		void initialize(InitializeStage stage) override;

		void addPropertyPages(PropertySheetDialog*) override;

		MultiDocInterface* createWidget(QWidget* parent) const override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getSvgIconResource() const override;

		void settingsReadWrite(bool save);

		DockWidgetList createDockingWidgets(QWidget* parent) override;

		void documentActivated(MultiDocInterface* interface, bool yn) const override;

		~LayoutEditorAppModule() override;

		QSettings* _settings;

		HierarchyViewer* _hierarchyViewer{nullptr};

		QTreeView* _tvProperties{nullptr};

		bool _readOnly = true;
};

}
