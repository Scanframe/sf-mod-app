#include <qmetaobject.h>
#include <QFrame>
#include <QBoxLayout>
#include <QSplitter>
#include <QBuffer>
#include <QStack>
#include <QDomDocument>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QLabel>
#include "ObjectExtension.h"
#include "FormBuilder.h"

namespace sf
{

bool isDispensablePropertyByName(QObject* obj, const QString& name)
{
	if (dynamic_cast<QWidget*>(obj))
	{
		static const char* keys[] = {
			"locale",
			"mouseTracking",
			"tabletTracking",
			"font",
			"windowIconText",
			"windowModified",
			"windowFilePath",
			"palette",
			// The cursor seems to be stored somehow in some cases, so we get rid of it completely
			"cursor",
		};
		// Check if passed property name is in the keys list.
		return std::any_of(&keys[0], &keys[sizeof(keys) / sizeof(keys[0])], [name](const char* prop)
		{
			return name == prop;
		});
	}
	return false;
}

bool isDispensablePropertyByValue(QObject* obj, const QString& name, const QVariant& variant)
{
	if (!variant.isValid() || variant.isNull())
	{
		return false;
	}
	// QLineEdit related.
	if (dynamic_cast<QLabel*>(obj))
	{
		if (name == "margin")
		{
			return variant.toInt() == 0;
		}
		if (name == "indent")
		{
			return variant.toInt() == -1;
		}
		if (name == "scaledContents" || name == "wordWrap" || name == "openExternalLinks")
		{
			return variant.toBool() == false;
		}
		if (name == "text")
		{
			return variant.toString().isEmpty();
		}
	}
	// QLineEdit related.
	if (dynamic_cast<QLineEdit*>(obj))
	{
		if (name == "modified")
		{
			// Not save this at all.
			return true;
		}
		if (name == "readOnly" || name == "clearButtonEnabled" || name == "dragEnabled")
		{
			return variant.toBool() == false;
		}
		if (name == "frame")
		{
			return variant.toBool() == true;
		}
		if (name == "inputMask" || name == "text" || name == "acceptDrops")
		{
			return variant.toString().isEmpty();
		}
		if (name == "maxLength")
		{
			return variant.toInt() == 32767;
		}
		if (name == "cursorPosition")
		{
			return variant.toInt() == 0;
		}
		if (name == "cursor")
		{
			return (variant.typeId() == QMetaType::QCursor)
				&& (qvariant_cast<Qt::CursorShape>(variant) == Qt::CursorShape::IBeamCursor);
		}
	}
	// QGridLayout related.
	if (dynamic_cast<QFormLayout*>(obj))
	{
		if (name == "horizontalSpacing" || name == "verticalSpacing")
		{
			return variant.toInt() == 6;
		}
	}
	// QGridLayout related.
	if (dynamic_cast<QGridLayout*>(obj))
	{
		if (name == "horizontalSpacing" || name == "verticalSpacing")
		{
			return variant.toInt() == 6;
		}
	}
	// QBoxLayout related.
	if (dynamic_cast<QBoxLayout*>(obj))
	{
		if (name == "leftMargin" || name == "rightMargin" || name == "topMargin" || name == "bottomMargin")
		{
			return variant.toInt() == 9;
		}
		if (name == "spacing")
		{
			return variant.toInt() == 6;
		}
		// Seems not implemented yet by Qt to export this one.
		if (name == "sizeConstraint")
		{
			return qvariant_cast<QLayout::SizeConstraint>(variant) == QLayout::SetDefaultConstraint;
		}
	}
	// QSplitter related.
	if (dynamic_cast<QSplitter*>(obj))
	{
		if (name == "opaqueResize")
		{
			return variant.toBool();
		}
		if (name == "handleWidth")
		{
			return variant.toInt() == 4;
		}
		if (name == "childrenCollapsible")
		{
			return variant.toBool() == true;
		}
	}
	// QFrame related.
	if (dynamic_cast<QFrame*>(obj))
	{
		if (name == "lineWidth")
		{
			return variant.toInt() == 1;
		}
		if (name == "midLineWidth")
		{
			return variant.toInt() == 0;
		}
	}
	// QPushButton related.
	if (dynamic_cast<QPushButton*>(obj))
	{
		// Bool false.
		if (name == "flat" || name == "autoDefault" || name == "default")
		{
			return variant.toBool() == false;
		}
	}
	// QToolButton related.
	if (dynamic_cast<QToolButton*>(obj))
	{
		// Bool false.
		if (name == "autoRaise")
		{
			return variant.toBool() == false;
		}
		if (name == "arrowType")
		{
			return qvariant_cast<Qt::ArrowType>(variant) == Qt::ArrowType::NoArrow;
		}
		if (name == "toolButtonStyle")
		{
			return qvariant_cast<Qt::ToolButtonStyle>(variant) == Qt::ToolButtonStyle::ToolButtonIconOnly;
		}
		if (name == "popupMode")
		{
			return qvariant_cast<QToolButton::ToolButtonPopupMode>(variant) == QToolButton::DelayedPopup;
		}
	}
	// QAbstractButton related.
	if (dynamic_cast<QAbstractButton*>(obj))
	{
		// QSize 16x16 properties.
		if (name == "iconSize")
		{
			return variant.toSize() == QSize(16, 16);
		}
		if (name == "autoRepeatDelay")
		{
			return variant.toInt() == 300;
		}
		if (name == "autoRepeatInterval")
		{
			return variant.toInt() == 100;
		}
		// QString empty properties.
		if (name == "text" || name == "shortcut")
		{
			return variant.toString().isEmpty();
		}
		// Bool false.
		if (name == "autoExclusive" || name == "checkable" || name == "down" || name == "checked" || name == "autoRepeat")
		{
			return variant.toBool() == false;
		}
	}
	// QWidget related.
	if (dynamic_cast<QWidget*>(obj))
	{
		// QString empty properties.
		if (name == "toolTip" || name == "windowTitle" || name == "statusTip" || name == "whatsThis" ||
			name == "styleSheet" || name == "accessibleName" || name == "accessibleDescription")
		{
			return variant.toString().isEmpty();
		}
		// Bool true properties.
		if (name == "enabled")
		{
			return variant.toBool() == true;
		}
		// Bool false properties.
		if (name == "autoFillBackground" || name == "acceptDrops")
		{
			return variant.toBool() == false;
		}
		// Double properties.
		if (name == "windowOpacity")
		{
			return variant.toDouble() == 1.0;
		}
		// Integer properties (-1).
		if (name == "toolTipDuration")
		{
			return variant.toInt() == -1;
		}
		// Integer properties (0).
		if (name == "minimumWidth" || name == "minimumHeight")
		{
			return variant.toInt() == 0;
		}
		// QSize properties.
		if (name == "baseSize" || name == "sizeIncrement" || name == "minimumSize")
		{
			return variant.toSize() == QSize(0, 0);
		}
		// QSize properties.
		if (name == "maximumSize")
		{
			return variant.toSize() == QSize(16777215, 16777215);
		}
		// QPoint properties.
		if (name == "pos")
		{
			return variant.toPoint() == QPoint(0, 0);
		}
		// QSizePolicy properties.
		if (name == "sizePolicy" && variant.canConvert<QSizePolicy>())
		{
			auto sp = qvariant_cast<QSizePolicy>(variant);
			bool rv = true;
			rv &= sp.horizontalStretch() == 0;
			rv &= sp.verticalStretch() == 0;
			rv &= sp.horizontalPolicy() == QSizePolicy::Preferred;
			rv &= sp.verticalPolicy() == QSizePolicy::Preferred;
			return rv;
		}
		// QCursor properties.
		if (name == "cursor")
		{
			return variant == QCursor(Qt::CursorShape::ArrowCursor);
		}
	}
	return false;
}

QList<DomProperty*> FormBuilder::computeProperties(QObject* obj)
{
	auto list = QFormBuilder::computeProperties(obj);
	// Associate the properties with their names.
	auto map = propertyMap(list);
	// Save erase function for when key does not exist.
	auto eraseProperty = [&map, obj](const QString& key, const char* reason) mutable
	{
		(void) obj;
		auto it = map.find(key);
		if (it != map.end())
		{
			map.erase(it);
		}
	};
	// Remove any undesired property.
	for (auto& key: map.keys())
	{
		// Get the property index.
		auto idx = obj->metaObject()->indexOfProperty(key.toLocal8Bit().constData());
		if (idx != -1)
		{
			if (!obj->metaObject()->property(idx).isStored() ||
				!obj->metaObject()->property(idx).isDesignable())
			{
				eraseProperty(key, "meta");
				continue;
			}
		}
		if (isDispensablePropertyByName(obj, key))
		{
			eraseProperty(key, "name");
			continue;
		}
		// Remove properties which has default values.
		if (isDispensablePropertyByValue(obj, key, obj->property(key.toLocal8Bit())))
		{
			eraseProperty(key, "value");
			continue;
		}
	}
	// Check if a widget parent exists.
	if (auto parent = dynamic_cast<QWidget*>(obj->parent()))
	{
		// When a parent layout is present or parent is a splitter.
		if (parent->layout() || dynamic_cast<QSplitter*>(parent))
		{
			// Remove sizing.
			for (auto& key: QStringList({"pos", "size", "geometry"}))
			{
				eraseProperty(key, "parent");
			}
		}
	}
	// When it exists modify the property list.
	if (auto oe = dynamic_cast<sf::ObjectExtension*>(obj))
	{
		for (auto& key: map.keys())
		{
			// When the object property is not required remove it from the list/map.
			if (!oe->isRequiredProperty(key))
			{
				eraseProperty(key, "custom");
			}
		}
	}
	return map.values();
}

QWidget* FormBuilder::load(QIODevice* dev, QWidget* parentWidget)
{
	// Create a dom to hold the XML.
	QDomDocument dom;
	// Read the dom contents from the device.
	dom.setContent(dev->readAll());
	// Root widget when loaded.
	QWidget* widget;
	// Make buffer go out of scope when done.
	{
		// Create an IO device for loading the form builder.
		QBuffer buf;
		// Open the buffer device.
		buf.open(QIODevice::ReadWrite);
		// Write the dom doc in the buffer.
		buf.write(dom.toByteArray());
		// Move pointer to start of the buffer for reading.
		buf.seek(0);
		// Load the UI XML from the buffer.
		widget = QFormBuilder::load(&buf, parentWidget);
	}
	// Check if the resulting widget is valid.
	if (widget)
	{
		fixLoadingProperties(widget, dom);
	}
	return widget;
}

void FormBuilder::save(QIODevice* dev, QWidget* widget)
{
	// Create to DOM document for reading the builder XML output.
	QDomDocument dom;
	// Scope for buffer to go out of when done.
	{
		// Write the UI file to a buffer device.
		QBuffer buf;
		// Open the buffer device.
		buf.open(QIODevice::WriteOnly);
		// Save the form to the buffer device.
		QFormBuilder::save(&buf, widget);
		// Assign the buffer to the XML document.
		dom.setContent(buf.data());
	}
	// Fix the missing or broken stuff.
	fixSavingProperties(widget, dom);
	// Write the document to file.
	dev->write(dom.toByteArray());

}

template<typename Func>
void iterateDomElements(QDomDocument& dom, const QDomElement& elem, Func callback = nullptr)
{
	// Return value.
	QStack<QDomElement> stack;
	//
	auto el = elem;
	// Loop as long as the element is valid.
	while (!el.isNull())
	{
		// When the callback is provided call otherwise add all elements.
		callback(el);
		// Move into the child nodes when there are any.
		if (!el.firstChildElement().isNull())
		{
			// Push the current element onto the stack.
			stack.push(el);
			// Assign the first element and restart the iteration.
			el = el.firstChildElement();
			//
			continue;
		}
		// Move to the next sibling.
		el = el.nextSiblingElement();
		// Check if there is a next sibling.
		if (!el.isNull())
		{
			continue;
		}
		while (el.isNull() && !stack.isEmpty())
		{
			// Pop the element, so we continue where we left of.
			el = stack.pop();
			// Move to the next one.
			el = el.nextSiblingElement();
		}
	}
}

template<typename Func>
QList<QDomElement> findDomElements(QDomDocument& dom, const QDomElement& elem, Func callback = nullptr)
{
	// Return value.
	QList<QDomElement> rv;
	// Iterate through the list of dom elements and add them to the return value list when the callback returns true.
	iterateDomElements(dom, elem, [&rv, callback](const QDomElement& el)
	{
		// When the callback is provided call otherwise add all elements.
		if (callback(el))
		{
			rv.append(el);
		}
	});
	return rv;
}

QString XmlPathName(const QDomNode& node)
{
	QStringList sl;
	//sl.prepend("#" + node.attributes().namedItem("name").nodeValue());
	auto n = node;
	while (!n.isNull())
	{
		sl.prepend(n.nodeName());
		n = n.parentNode();
	}
	return sl.join("\\");
}

QDomElement createDomProperty(QDomDocument& dom, const QString& name, const QString& type, const QString& value)
{
	auto prop = dom.createElement("property");
	auto attr = dom.createAttribute("name");
	attr.setValue(name);
	prop.setAttributeNode(attr);
/*
 * This is not working where this works everywhere else.
	prop.setAttributeNode(dom.createAttribute("name")).setValue(name);
*/
	auto enu = dom.createElement(type);
	prop.appendChild(enu);
	enu.appendChild(dom.createTextNode(value));
	return prop;
}

void FormBuilder::fixSavingProperties(QWidget* widget, QDomDocument& dom)
{
	// List of class names not saving any children.
	QStringList classNames;
	// Iterate to gather the class names not saving children.
	for (auto w: widget->findChildren<QWidget*>())
	{
		if (auto oe = dynamic_cast<ObjectExtension*>(w))
		{
			if (!oe->getSaveChildren())
			{
				if (!classNames.contains(w->metaObject()->className()))
				{
					classNames.append(w->metaObject()->className());
				}
			}
		}
	}
	// Get the root widget element.
	auto root = dom.firstChildElement("ui").firstChildElement("widget");
	// Fix custom container widgets.
	{
		// Iterate through all dom elements
		iterateDomElements(dom, root, [&](QDomElement& elem)
		{
			if (elem.nodeName() == "widget")
			{
				// Get the name attribute of the widget element.
				auto attr = elem.attributes().namedItem("class");
				// Check if the attribute is valid attribute and the name is not empty.
				if (!attr.isNull() && attr.isAttr() && !attr.nodeValue().isEmpty())
				{
					if (classNames.contains(attr.nodeValue()))
					{
						// Remove the layout which has the widgets when it exists.
						elem.removeChild(elem.firstChildElement("layout"));
						// Remove all child widgets.
						{
							// Remove all child widgets.
							auto child = elem.firstChildElement("widget");
							while (!child.isNull())
							{
								elem.removeChild(child);
								// Move to next which is now the first again.
								child = elem.firstChildElement("widget");
							}
						}
						// Remove all actions elements.
						{
							auto action = elem.firstChildElement("action");
							while (!action.isNull())
							{
								elem.removeChild(action);
								// Move to next which is now the first again.
								action = elem.firstChildElement("action");
							}
						}
					}
				}
			}
		});
	}
	// Fix QWidget or derived elements custom properties.
	{
		// Iterate through all dom elements
		iterateDomElements(dom, root, [&dom, widget](QDomElement& elem)
		{
			// Only check widget dom elements.
			if (elem.nodeName() == "widget")
			{
				// Get the name attribute of the widget element.
				auto attr = elem.attributes().namedItem("name");
				// Check if the attribute is valid attribute and the name is not empty.
				if (!attr.isNull() && attr.isAttr() && !attr.nodeValue().isEmpty())
				{
					auto wgt_name = attr.nodeValue();
					// Find the corresponding widget using the name.
					if (auto wgt = (wgt_name == widget->objectName()) ? widget : widget->findChild<QWidget*>(wgt_name))
					{
						// Iterate through the dynamically added property names.
						for (auto& pnm: wgt->dynamicPropertyNames())
						{
							auto prop = wgt->property(pnm);
							if (!prop.isNull() && prop.typeId() == QMetaType::QString)
							{
								elem.insertAfter(createDomProperty(dom, pnm, "string", prop.toString()),
									elem.lastChildElement("property"));
							}
						}
					}
				}
			}
		});
	}
	// Fix QFrame or derived widget elements shadow and shape properties.
	{
		QMap<QString, QFrame*> frames;
		QStringList classes;
		// Get all widgets derived from a frame.
		for (auto frame: widget->findChildren<QFrame*>(QString(), Qt::FindChildrenRecursively))
		{
			classes.append(frame->metaObject()->className());
			if (!frame->objectName().isEmpty())
			{
				frames[frame->objectName()] = frame;
			}
		}
		// QMetaEnum::fromType
		auto metaEnumShadow = QMetaEnum::fromType<QFrame::Shadow>();
		auto metaEnumShape = QMetaEnum::fromType<QFrame::Shape>();
		// Get list of all widgets.
		iterateDomElements(dom, root, [&](QDomElement& elem)
		{
			// All widget elements meeting the class names.
			if (elem.nodeName() == "widget" && classes.contains(elem.attributes().namedItem("class").nodeValue()))
			{
				auto name = elem.attributes().namedItem("name").nodeValue();
				//
				if (!name.isEmpty() && frames.contains(name))
				{
					auto frm = frames[name];
					// Defaults for a QFrame.
					QFrame::Shadow def_shadow = QFrame::Raised;
					QFrame::Shape def_shape = QFrame::StyledPanel;
					// Set other defaults for a QLabel.
					if (dynamic_cast<QLabel*>(frm))
					{
						def_shadow = QFrame::Plain;
						def_shape = QFrame::NoFrame;
					}
					// Only add property when different from the default.
					if (def_shape != frm->frameShape())
					{
						elem.insertAfter(createDomProperty(dom, "frameShadow", "enum",
								QString("QFrame::%1").arg(metaEnumShadow.valueToKey(frm->frameShadow()))),
							elem.lastChildElement("property"));
					}
					// Only add property when different from the default.
					if (def_shadow != frm->frameShadow())
					{
						elem.insertAfter(createDomProperty(dom, "frameShape", "enum",
								QString("QFrame::%1").arg(metaEnumShape.valueToKey(frm->frameShape()))),
							elem.lastChildElement("property"));
					}
				}
				// Fix QLabel's buddy. Which seems to be fixed already.
				// TODO: When the buddy exists this will add a second entry so check first if the buddy is present.
				if (auto lbl = dynamic_cast<QLabel*>(frames[name]))
				{
					if (lbl->buddy())
					{
						elem.insertAfter(createDomProperty(dom, "buddy", "cstring", lbl->buddy()->objectName()),
							elem.lastChildElement("property"));
					}
				}
			}
		});
	}
	// Fix layout elements.
	{
		QMap<QString, QLayout*> layouts;
		// Get all widgets derived from a frame.
		for (auto layout: widget->findChildren<QLayout*>(QString(), Qt::FindChildrenRecursively))
		{
			if (!layout->objectName().isEmpty())
			{
				layouts[layout->objectName()] = layout;
			}
		}
		// Initialize with the default values for frames.
		QMargins defaults(9, 9, 9, 9);
		// iterate through the list dom elements
		iterateDomElements(dom, root, [&](QDomElement& elem)
		{
			// Only layout named elements apply.
			if (elem.nodeName() == "layout")
			{
				//
				auto name = elem.attributes().namedItem("name").nodeValue();
				/*
				 * TODO: It seems that the default margins from the root widget are not honored when reading the file.
				 * Solution is to write them into the XML file before reading.
				 */
				// Make the root widget force write the margin properties.
				bool force = widget->layout() && widget->layout()->objectName() == name;
				//
				if (!name.isEmpty() && layouts.contains(name))
				{
					auto margins = layouts[name]->contentsMargins();
					// When the margins are not equal the defaults.
					if (margins.top() != defaults.top() || force)
					{
						elem.insertBefore(createDomProperty(dom, "topMargin", "number",
							QString("%1").arg(margins.top())), elem.firstChildElement());
					}
					if (margins.bottom() != defaults.bottom() || force)
					{
						elem.insertBefore(createDomProperty(dom, "bottomMargin", "number",
							QString("%1").arg(margins.bottom())), elem.firstChildElement());
					}
					if (margins.left() != defaults.left() || force)
					{
						elem.insertBefore(createDomProperty(dom, "leftMargin", "number",
							QString("%1").arg(margins.left())), elem.firstChildElement());
					}
					if (margins.right() != defaults.right() || force)
					{
						elem.insertBefore(createDomProperty(dom, "rightMargin", "number",
							QString("%1").arg(margins.right())), elem.firstChildElement());
					}
				}
			}
		});
	}
	// Fix tool button elements.
	{
		QMap<QString, QToolButton*> buttons;
		// Get all widgets derived from a frame.
		for (auto button: widget->findChildren<QToolButton*>(QString(), Qt::FindChildrenRecursively))
		{
			if (!button->objectName().isEmpty())
			{
				buttons[button->objectName()] = button;
			}
		}
		// Iterate through all dom elements
		iterateDomElements(dom, root, [widget, &dom, &buttons](QDomElement& elem)
		{
			// Only check widget dom elements.
			if (elem.nodeName() == "widget")
			{
				// Get the name attribute of the widget element.
				auto attr = elem.attributes().namedItem("name");
				// Check if the attribute is valid attribute and the name is not empty.
				if (!attr.isNull() && !attr.nodeValue().isEmpty())
				{
					auto name = attr.nodeValue();
					// When in the buttons map .
					if (!name.isEmpty() && buttons.contains(name))
					{
						// Add the missing property toolButtonStyle.
						auto style = QMetaEnum::fromType<Qt::ToolButtonStyle>().valueToKey(buttons[name]->toolButtonStyle());
						elem.insertBefore(createDomProperty(dom, "toolButtonStyle", "enum", style), elem.firstChildElement());
						auto arrow = QMetaEnum::fromType<Qt::ArrowType>().valueToKey(buttons[name]->arrowType());
						elem.insertBefore(createDomProperty(dom, "arrowType", "enum", arrow), elem.firstChildElement());
					}
				}
			}
		});
	}
}

void FormBuilder::fixLoadingProperties(QWidget* widget, QDomDocument& dom)
{
	// Create property name cache for each specific class.
	QMap<QString, QStringList> cache;
	// Get the root widget element.
	auto root = dom.firstChildElement("ui").firstChildElement("widget");
	// Fix QWidget or derived elements custom properties.
	{
		// Iterate through all dom elements
		iterateDomElements(dom, root, [widget, &cache](const QDomElement& elem)
		{
			// Only check widget dom elements.
			if (elem.nodeName() == "widget")
			{
				// Get the name attribute of the widget element.
				auto attr = elem.attributes().namedItem("name");
				// Check if the attribute is valid attribute and the name is not empty.
				if (!attr.isNull() && !attr.nodeValue().isEmpty())
				{
					auto wgt_name = attr.nodeValue();
					// Get a child by name if it is not the root widget itself.
					if (auto wgt = (wgt_name == widget->objectName()) ? widget : widget->findChild<QWidget*>(wgt_name))
					{
						// Get the widgets meta-object.
						auto mo = wgt->metaObject();
						// When the cache entry does not exist for this class name yet created it.
						if (!cache.contains(mo->className()))
						{
							// Easy access reference to the list.
							QStringList& nms = cache[mo->className()];
							// Iterate through its properties to collect names.
							for (int idx = 0; idx < mo->propertyCount(); ++idx)
							{
								// Add each known property to the string list.
								nms.append(mo->property(idx).name());
							}
						}
						// Get all non-dynamic properties in a list for comparison.
						QStringList& names = cache[mo->className()];
						// Iterate through property elements in the dom.
						for (auto& pnm: wgt->dynamicPropertyNames())
						{
							// Get the first property element.
							auto pel = elem.firstChildElement("property");
							while (!pel.isNull())
							{
								// Get the property name attribute of the property dom element.
								auto pna = pel.attributes().namedItem("name");
								// Check its validity.
								if (!pna.isNull())
								{
									// Get the property's n ode attributes node value.
									auto anv = pna.nodeValue();
									// Check if the name is not in the list of non-dynamic properties.
									if (!anv.isEmpty() && !names.contains(pna.nodeValue()))
									{
										auto pnv = pel.firstChildElement().text();
										wgt->setProperty(anv.toLocal8Bit(), pnv);
									}
								}
								// Go to the next property of this widget element.
								pel = pel.nextSiblingElement("property");
							}
						}
					}
				}
			}
		});
	}
}

}
