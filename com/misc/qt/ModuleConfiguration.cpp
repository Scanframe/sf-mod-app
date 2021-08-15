#include <QDir>
#include <QCoreApplication>
#include <QLibrary>
#include "../gen/TClassRegistration.h"
#include "ModuleConfiguration.h"
#include "ModuleConfigurationDialog.h"

namespace sf
{

inline
static QString getGroupName()
{
	return QStringLiteral("AppModules");
}

ModuleConfiguration::ModuleConfiguration(QSettings* settings, QObject* parent)
	:QObject(parent)
	 , _dialog(nullptr)
	 , _settings(settings)
{
}

ModuleConfiguration::~ModuleConfiguration()
{
	delete _dialog;
}

void ModuleConfiguration::openDialog(QWidget* parent)
{
	// Create a new dialog when needed.
	if (!_dialog)
	{
		_dialog = new ModuleConfigurationDialog(this, parent);
		// On close null the pointer.
		QObject::connect(_dialog, &ModuleConfigurationDialog::finished, [&](int) -> void
		{
			delete _dialog;
			_dialog = nullptr;
		});
	}
	// Show the dialog.
	_dialog->show();
	// Bring to front in case of having no parent.
	_dialog->raise();
}


QString ModuleConfiguration::getModuleDir()
{
	return QCoreApplication::applicationDirPath();
}

ModuleConfiguration::ModuleListType ModuleConfiguration::getList() const
{
	ModuleListType ml;
	_settings->beginGroup(getGroupName());
	// Remove the existing keys.
	for (auto& key: _settings->allKeys())
	{
		ml[key] = _settings->value(key).toString();
	}
	_settings->endGroup();
	return ml;
}

size_t ModuleConfiguration::load()
{
	// Return value initialization.
	size_t rv = 0;
	// Get the list of to be loaded modules.
	auto list = getList();
	// Load each dynamic library in the list when not loaded yet.
	for (auto it = list.begin(); it != list.constEnd(); ++it)
	{
		QLibrary lib(QDir(getModuleDir()).filePath(it.key()));
		// Only load when not loaded yet.
		if (!lib.isLoaded())
		{
			qInfo() << "Loading Module:" << it.key() << it.value();
			if (!lib.load())
			{
				qWarning() << it.key() << lib.errorString();
			}
			// Cast the function to the correct type.
			auto func = (SF_DL_NAME_FUNC_TYPE) lib.resolve(SF_DL_NAME_FUNC_NAME);
			if (func)
			{
				func(it.key().toStdString().c_str());
			}
			// Increment the return value.
			rv++;
		}
	}
	// When a lib was loaded emit the signal.
	if (rv)
	{
		emit libraryLoaded();
	}
	// Signal the caller a library was loaded.
	return rv;
}

void ModuleConfiguration::save(const ModuleConfiguration::ModuleListType& list)
{
	if (_settings)
	{
		_settings->beginGroup(getGroupName());
		// Remove the existing keys.
		for (auto& key: _settings->allKeys())
		{
			_settings->remove(key);
		}
		// Save each entry.
		for (auto i = list.begin(); i != list.constEnd(); ++i)
		{
			_settings->setValue(i.key(), i.value());
		}
		_settings->endGroup();
	}
}

}
