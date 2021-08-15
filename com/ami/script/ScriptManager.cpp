#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QException>
#include <gii/gen/GiiScriptInterpreter.h>
#include "ScriptManager.h"

namespace sf
{

bool ScriptEntry::start()
{
	QFileInfo fi(_manager->getScriptFilePath(_filename));
	// Sanity check.
	if (!QDir(fi.absoluteDir()).exists())
	{
		qWarning() << "Directory does not exist: " << fi.absoluteDir();
		return false;
	}
	// Sanity check.
	if (!fi.exists())
	{
		qWarning() << "Missing file:" << fi.fileName();
		return false;
	}
	QFile file(fi.absoluteFilePath());
	if (!file.open(QIODeviceBase::ReadOnly))
	{
		qWarning() << "File not opening:" << file.fileName();
		return false;
	}
	if (!_interpreter->compile(file.readAll()))
	{
		qWarning() << "Compile failed:" << getDisplayName() << file.fileName();
		return false;
	}
	if (!_interpreter->Execute(ScriptInterpreter::EExecMode::emInit))
	{
		qWarning() << "Initialization failed:" << getDisplayName() << file.fileName();
		return false;
	}
	// When in background is enabled and the script was initialized.
	if (_background && _interpreter->getState() == ScriptInterpreter::esReady)
	{
		// Move to the running state using start.
		if (_interpreter->Execute(ScriptInterpreter::emStart) == ScriptInterpreter::esError)
		{
			return false;
		}
	}
	return true;
}

void ScriptEntry::stop()
{
	_interpreter->flush();
}

QString ScriptEntry::getStateName() const
{
	return _interpreter.isNull() ? "?" : _interpreter->getStateName();
}

void ScriptEntry::setKeySequence(const QKeySequence& ks)
{
	if (_keySequence != ks)
	{
		_keySequence = ks;
		delete_null(_globalShortcut);
		_globalShortcut = new GlobalShortcut(ks, _manager);
		connect(_globalShortcut, &GlobalShortcut::activated, [&](GlobalShortcut* gsc)
		{
			_manager->shortcutActivated(this);
		});
		_manager->setModified(this);
	}
}

const QKeySequence& ScriptEntry::getKeySequence() const
{
	return _keySequence;
}

void ScriptEntry::setDisplayName(const QString& name)
{
	if (!_interpreter.isNull())
	{
		if (_interpreter->getScriptName() != name.toStdString())
		{
			_interpreter->setScriptName(name.toStdString());
			_manager->setModified(this);
		}
	}
}

QString ScriptEntry::getDisplayName() const
{
	if (!_interpreter.isNull())
	{
		return QString::fromStdString(_interpreter->getScriptName());
	}
	return {"?"};
}

void ScriptEntry::setGlobal(bool global)
{
	if (this->_global != global)
	{
		_global = global;
		if (_globalShortcut)
		{
			_globalShortcut->setGlobal(this->_global);
			_manager->setModified(this);
		}
	}
}

bool ScriptEntry::isGlobal() const
{
	return this->_global;
}

QString ScriptEntry::getScriptName() const
{
	if (_interpreter.isNull())
	{
		return {"?"};
	}
	return QString::fromStdString(_interpreter->getScriptName());
}

void ScriptEntry::setScriptName(const QString& name)
{
	if (!_interpreter.isNull())
	{
		if (_interpreter->getScriptName() != name.toStdString())
		{
			_interpreter->setScriptName(name.toStdString());
			_manager->setModified(this);
		}
	}
}

void ScriptEntry::setFilename(const QString& name)
{
	if (_filename != name)
	{
		_filename = name;
		_manager->setModified(this);
	}
}

void ScriptEntry::setBackgroundMode(ScriptEntry::EBackgroundMode bm)
{
	if (_background != bm)
	{
		_background = bm;
		_manager->setModified(this);
	}
}

ScriptManager::ScriptManager(QSettings* settings, QObject* parent)
	:QObject(parent)
	 , _settings(settings)
	 , _subDirectory("scripts")
	 , _timer(new QTimer(this))
{
	connect(_timer, &QTimer::timeout, this, &ScriptManager::backgroundRun);
	_timer->start();
}

ScriptManager::~ScriptManager()
{
	for (auto i: _list)
	{
		delete i;
	}
}

QString ScriptManager::getFileSuffix()
{
	return {"nsf"};
}

QString ScriptManager::getScriptFilePath(const QString& base_name) const
{
	const char* prop = "ConfigDir";
	auto cfg_dir = QApplication::instance()->property(prop);
	if (!cfg_dir.isValid())
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Application property '%s' not valid!", prop);
	}
	else
	{
		QString rv = cfg_dir.toString() + QDir::separator() + _subDirectory;
		if (!base_name.isEmpty())
		{
			rv += QDir::separator() + base_name + '.' + getFileSuffix();
		}
		return rv;
	}
}

void ScriptManager::settingsReadWrite(bool save)
{
	_settings->beginGroup("ScriptManager");
	if (save)
	{
		// Only save when modified.
		if (_modified)
		{
			_settings->setValue("ScriptDir", _subDirectory);
			// Reset the modified flag.
			_modified = false;
			//
			_settings->beginWriteArray("Entries", static_cast<int>(_list.size()));
			int index = 0;
			for (auto& entry: _list)
			{
				_settings->setArrayIndex(index++);
				_settings->setValue("DisplayName", entry->getDisplayName());
				_settings->setValue("Shortcut", entry->getKeySequence().toString());
				_settings->setValue("Global", entry->isGlobal());
				_settings->setValue("Background", entry->getBackgroundMode());
				_settings->setValue("Filename", entry->getFilename());
			}
			_settings->endArray();
			// Timer interval value om msecs.
			_settings->setValue("TimerInterval", _timer->interval());
		}
	}
	else
	{
		_subDirectory = _settings->value("ScriptDir", _subDirectory).toString();
		auto sz = _settings->beginReadArray("Entries");
		for (int i = 0; i < sz; i++)
		{
			_settings->setArrayIndex(i);
			auto entry = new ScriptEntry(this);
			entry->setInterpreter(QSharedPointer<ScriptInterpreter>(new GiiScriptInterpreter()));
			entry->setBackgroundMode(qvariant_cast<ScriptEntry::EBackgroundMode>(_settings->value("Background", entry->getBackgroundMode())));
			entry->setDisplayName(_settings->value("DisplayName", QString("Script-%1").arg(i)).toString());
			entry->setKeySequence(QKeySequence::fromString(_settings->value("Shortcut").toString()));
			entry->setGlobal(_settings->value("Global").toBool());
			entry->setFilename(_settings->value("Filename").toString());
			_list.append(entry);
		}
		_settings->endArray();
		// Set the timer.
		_timer->setInterval(_settings->value("TimerInterval", 333).toInt());
		// Just reset the modified flag after restoring the entries.
		_modified = false;
	}
	_settings->endGroup();
}

bool ScriptManager::isModified() const
{
	return _modified;
}

void ScriptManager::addAt(qsizetype index)
{
	auto entry = new ScriptEntry(this);
	entry->setInterpreter(QSharedPointer<ScriptInterpreter>(new GiiScriptInterpreter()));
	entry->setScriptName("new");
	// When index is less than zero append a new entry.
	if (index < 0)
	{
		_list.append(entry);
	}
	else
	{
		_list.insert(index, entry);
	}
	// Configuration needs to be saved.
	setModified(this);
}

void ScriptManager::remove(qsizetype index)
{
	// Check if index is in bounds.
	if (index >= 0 && index < _list.size())
	{
		// Delete the entry first before removing.
		delete_null(_list[index]);
		// Remove the pointer from the list.
		_list.remove(index);
		// Configuration needs to be saved.
		_modified = true;
	}
}

void ScriptManager::start(qsizetype index)
{
	// When index is -1 start all scripts.
	if (index < 0)
	{
		for (auto entry: _list)
		{
			entry->start();
		}
	}
	// Check if index is in bounds.
	if (index >= 0 && index < _list.size())
	{
		// Start a single script.
		_list[index]->start();
	}
}

void ScriptManager::stop(qsizetype index)
{
	// When index is -1 stop all scripts.
	if (index < 0)
	{
		for (auto entry: _list)
		{
			entry->stop();
		}
	}
	// Check if index is in bounds.
	if (index >= 0 && index < _list.size())
	{
		// Stop a single script.
		_list[index]->stop();
	}
}

void ScriptManager::backgroundRun()
{
	for (auto entry: _list)
	{
		if (!entry->getInterpreter().isNull())
		{
			auto interpreter = entry->getInterpreter().get();
			switch (entry->getBackgroundMode())
			{
				case ScriptEntry::bmNo:
					break;

				case ScriptEntry::bmContinuous:
					if (interpreter->getState() == ScriptInterpreter::esReady)
					{
						if (interpreter->Execute(ScriptInterpreter::emStart) != ScriptInterpreter::esError)
						{
							std::clog << "Start succeeded." << std::endl;
						}
						else
						{
							std::clog << "Start failed: (" << interpreter->getError() << ") " << interpreter->getErrorReason() << std::endl;
						}
					}
					// Run into next.

				case ScriptEntry::bmOnce:
					if (interpreter->getState() == ScriptInterpreter::esRunning)
					{
						interpreter->Execute(ScriptInterpreter::emStep);
					}
					break;
			}
		}
	}
}

ScriptEntry* ScriptManager::getEntry(qsizetype index)
{
	if (index >= 0 && index < _list.size())
	{
		return _list[index];
	}
	return nullptr;
}

QStringList ScriptManager::getFilenames() const
{
	QStringList rv;
	auto suffix = getFileSuffix().prepend('.');
	QDirIterator it(getScriptFilePath(), {QString("*").append(suffix)}, QDir::Files);
	while (it.hasNext())
	{
		it.next();
		auto s = it.fileName();
		rv.append(s.left(s.length() - suffix.length()));
	}
	return rv;
}

void ScriptManager::shortcutActivated(ScriptEntry* entry)
{
	entry->getInterpreter()->callFunction("main", entry->getBackgroundMode() != ScriptEntry::bmNo);
}

void ScriptManager::setModified(QObject* caller)
{
	(void) caller;
	_modified = true;
}

const ScriptEntry* ScriptManager::getEntryByFilepath(const QString& filepath)
{
	for (auto entry: _list)
	{
		if (filepath == getScriptFilePath(entry->getFilename()))
		{
			return entry;
		}
	}
	return nullptr;
}

}
