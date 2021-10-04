# Try this example


[Task Menu Extension](https://doc.qt.io/qt-6/qtdesigner-taskmenuextension-example.html)


Files are located [here](https://code.qt.io/cgit/qt/qttools.git/tree/examples/designer/taskmenuextension?h=6.2)



```c++
#include "MyGlobal.h"

Q_PROPERTY(MyGlobal::Custom discovery READ discovery WRITE setDiscovery STORED true)
//Q_PROPERTY(MyGlobal::Custom discovery READ discovery WRITE setDiscovery STORED true)
Q_PROPERTY(QString theString MEMBER _theString STORED true)

[[nodiscard]] MyGlobal::Custom discovery() const { return _discovery; }
void setDiscovery(const MyGlobal::Custom& c) { _discovery = c; }
MyGlobal::Custom _discovery;
QString _theString;


		
void AnalogClockPlugin::initialize(QDesignerFormEditorInterface* core)
{
	Q_UNUSED(core)
	if (initialized)
	{
		return;
	}

	QExtensionManager *manager = core->extensionManager();
	Q_ASSERT(manager != nullptr);

	manager->registerExtensions(new PropertyExtensionFactory(manager), Q_TYPEID(QDesignerPropertySheetExtension));

	initialized = true;
}
		
		
		
```