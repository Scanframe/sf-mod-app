#include <QMimeDatabase>
#include "EditorAppModule.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Text Editor",
	R"(Editors MDI for code and text using syntaxt highlighting.)"
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	EditorAppModule, "Class-B", "Actual name of the class."
)

EditorAppModule::EditorAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
{
}

void EditorAppModule::addPropertySheets(QWidget*)
{
}

bool EditorAppModule::isHandlingMime(const QMimeType& mime) const
{
	// Check if the mime is valid and bailout if not.
	if (!mime.isValid())
	{
		return false;
	}
	// Return true when the mime is handled.
	return _handledMimeTypes.contains(mime.name());
}

bool EditorAppModule::doInitialize(bool initialize) noexcept
{
	if (initialize)
	{
		if (_handledMimeTypes.isEmpty())
		{
			QMimeDatabase mdb;
			_handledMimeTypes.insert(mdb.mimeTypeForName("text/javascript").name(), true);
			_handledMimeTypes.insert(mdb.mimeTypeForName("text/plain").name(), false);
		}
	}
	return true;
}

};