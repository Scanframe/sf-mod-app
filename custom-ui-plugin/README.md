# Custom UI Plugins

Creating a custom widget plugins for the Qt Designer.
For having multiple widgets in a single dynamic library (plugin) interface 
[QDesignerCustomWidgetCollectionInterface](https://doc.qt.io/qt-5/qdesignercustomwidgetcollectioninterface.html)
needs to be implemented in order to do this.

### Plugin 1: Analog Clock Example

Created from the QT [example](https://doc.qt.io/qt-5/qtdesigner-customwidgetplugin-example.html).
Simple widget drawing a analog clock.

### Plugin 2: Extended PushButton

This widget is created from an existing [QPushButton](https://doc.qt.io/qt-5/qpushbutton.html).
Adds a property for setting icon from resource or external file. 
