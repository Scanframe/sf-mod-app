#pragma once

#include <QMap>
#include <ami/iface/AppModuleInterface.h>

namespace sf
{

class [[maybe_unused]] EditorAppModule :public AppModuleInterface
{
	public:

		explicit EditorAppModule(const Parameters& params);

		void addPropertySheets(QWidget*) override;

		[[nodiscard]] bool isHandlingMime(const QMimeType& mime) const override;

	protected:

		bool doInitialize(bool initialize) noexcept override;

	private:

		QMap<QString, bool> _handledMimeTypes;

};

}
