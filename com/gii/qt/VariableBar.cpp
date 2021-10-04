#include "VariableBar.h"
#include <QApplication>
#include <QMouseEvent>
#include <QStyle>
#include <QVBoxLayout>
#include <QStylePainter>
#include <QStyleOption>
#include <QPainter>
#include <QTimer>
#include <misc/qt/qt_utils.h>
#include "VariableWidgetBasePrivate.h"

namespace sf
{

struct VariableBar::Private :QObject, PrivateBase
{
	int margin{1};

	VariableBar* _widget{nullptr};
	QLabel* _labelNameAlt{nullptr};
	int _nameLevel{-1};

	static Private* cast(PrivateBase* data)
	{
		return static_cast<Private*>(data); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	}

	explicit Private(VariableBar* widget)
		:_widget(widget)
	{
		// Make the widget get focus when clicked in.
		_widget->setFocusPolicy(Qt::StrongFocus);
		// Also, when using the keyboard.
		_widget->setAttribute(Qt::WA_KeyboardFocusChange);
		// Only link when not in design mode.
		if (!ObjectExtension::inDesigner())
		{
			_variable.setHandler(this);
		}
		QTimer::singleShot(0, this, &VariableBar::Private::connectLabelNameAlt);
	}

	void onDestroyed(QObject* obj = nullptr) // NOLINT(readability-make-member-function-const)
	{
		if (_labelNameAlt && _labelNameAlt == obj)
		{
			disconnect(_labelNameAlt, &QLabel::destroyed, this, &VariableBar::Private::onDestroyed);
			_labelNameAlt = nullptr;
		}
	}

	void connectLabelNameAlt()
	{
		// Try finding label where this widget is its buddy.
		if (auto label = findLabelByBuddy(_widget))
		{
			// Assign the pointer to alternate name label.
			_labelNameAlt = label;
			// Connect handler for when the label is destroyed to null the alternate label.
			connect(_labelNameAlt, &QLabel::destroyed, this, &VariableBar::Private::onDestroyed);
			// Trigger event to fill in the label text and tool tip.
			_variable.emitEvent(Variable::veUserPrivate);
		}
	}

	~Private() override
	{
		// Clear the handler when destructing.
		_variable.setHandler(nullptr);
	}

	void variableEventHandler
		(
			EEvent event,
			const Variable& call_var,
			Variable& link_var,
			bool same_inst
		) override
	{
		switch (event)
		{
			case veLinked:
			case veIdChanged:
				_widget->applyReadOnly(_readOnly || link_var.isReadOnly());
				_widget->setToolTip(QString::fromStdString(link_var.getDescription()));
				_widget->update();
				// Run into next.
			case veUserPrivate:
				if (_labelNameAlt)
				{
					_labelNameAlt->setToolTip(_widget->toolTip());
					_labelNameAlt->setText(QString::fromStdString(link_var.getName(_nameLevel)));
				}
				break;

			case veValueChange:
			case veConverted:
				_widget->update();
				break;

			default:
				break;
		}
	}

	[[nodiscard]] QRect clipRect() const
	{
		const QWidget* w = _widget;
		if (!w->isVisible())
		{
			return {};
		}
		QRect r = _widget->rect();
		int ox = 0;
		int oy = 0;
		while (w->isVisible() && !w->isWindow() && w->parentWidget())
		{
			ox -= w->x();
			oy -= w->y();
			w = w->parentWidget();
			r &= QRect(ox, oy, w->width(), w->height());
		}
		return r;
	}
};

VariableBar::VariableBar(QWidget* parent)
	:VariableWidgetBase(parent, this)
{
	_p = new Private(this);
}

bool VariableBar::isRequiredProperty(const QString& name)
{
	return true;
}

void VariableBar::applyReadOnly(bool yn)
{
	setFocusPolicy(yn ? Qt::NoFocus : Qt::StrongFocus);
}

QSize VariableBar::minimumSizeHint() const
{
	auto p = Private::cast(_p);
	// Not sure if sizeHint() needs to be implemented.
	ensurePolished();
	QFontMetrics fm(fontMetrics());
	QStyleOptionFrame opt;
	initStyleOption(&opt);
	int h = fm.height() + 2 * p->margin;
	int w = fm.maxWidth() + 2 * p->margin;
	// When a no style object is present set line width to zero to get the correct calculation.
	// A style object is present when a style has been applied using a stylesheet.
	if (!opt.styleObject)
	{
		opt.lineWidth = 0;
	}
	return style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h), this);
}

void VariableBar::initStyleOption(QStyleOptionFrame* option) const
{
	if (!option)
	{
		return;
	}
	auto p = Private::cast(_p);
	option->initFrom(this);
	option->rect = contentsRect();
	// Needed otherwise no focus rectangle is drawn.
	option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
	option->midLineWidth = 0;
	option->state |= QStyle::State_Sunken;
	if (p->_readOnly || p->_variable.isReadOnly())
	{
		option->state |= QStyle::State_ReadOnly;
	}
	option->features = QStyleOptionFrame::None;
}

void VariableBar::paintEvent(QPaintEvent* event)
{
	VariableWidgetBase::paintEvent(event);
	auto p = Private::cast(_p);
	QStyleOptionFrame panel;
	initStyleOption(&panel);
	// Create painter for this widget.
	QStylePainter sp(this);
	// Use base function to draw the focus rectangle having the right color as a regular edit widget.
	sp.drawPrimitive(QStyle::PE_PanelLineEdit, panel);
	auto rc = sp.style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
	// Create 1 pixel spacing between border and
	inflate(rc, -p->margin);
	// Set designer text as default.
	QString text("Value Unit");
	auto pos = rc.size().width() / 2;
	if (!inDesigner())
	{
		// Get variable as a reference.
		auto& v(p->_variable);
		text = QString::fromStdString(v.getCurString() + ' ' + v.getUnit());
		pos = (int) Value::calculateOffset(v.getCur(), v.getMin(), v.getMax(), Value(rc.width()), true).getInteger();
	}
	// First part.
	sp.setClipRect(QRect(rc.topLeft(), QSize(pos, rc.height())));
	sp.setBrush(palette().color(QPalette::Highlight));
	sp.setPen(Qt::NoPen);
	sp.drawRect(rc);
	sp.setClipRect(rc);
	sp.setPen(QPen(palette().color(QPalette::HighlightedText)));
	sp.drawText(rc, Qt::AlignCenter, text);
	// Second part.
	sp.setClipRect(QRect(QPoint(pos, 0), rc.bottomRight()));
	sp.setBrush(palette().color(QPalette::Base));
	sp.setPen(Qt::NoPen);
	sp.drawRect(rc);
	sp.setPen(QPen(palette().color(QPalette::Text)));
	sp.drawText(rc, Qt::AlignCenter, text);
}

void VariableBar::keyPressEvent(QKeyEvent* event)
{
	_p->keyPressEvent(event);
}

int VariableBar::nameLevel() const
{
	return VariableBar::Private::cast(_p)->_nameLevel;
}

void VariableBar::setNameLevel(int level)
{
	auto p = VariableBar::Private::cast(_p);
	if (p->_nameLevel != level)
	{
		p->_nameLevel = level;
		p->_variable.emitEvent(Variable::veUserPrivate);
	}
}

}
