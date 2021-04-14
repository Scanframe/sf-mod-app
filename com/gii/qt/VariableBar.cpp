#include "VariableBar.h"

#include <QApplication>
#include <QMouseEvent>
#include <QStyle>
#include <QVBoxLayout>
#include <QStylePainter>
#include <QStyleOption>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <misc/qt/qt_utils.h>
#include <misc/gen/dbgutils.h>
#include "VariableWidgetBasePrivate.h"

namespace sf
{

struct VariableBarPrivate :VariableWidgetBasePrivate
{
	VariableBar* _widget{nullptr};

	static VariableBarPrivate* cast(VariableWidgetBasePrivate* data)
	{
		return static_cast<VariableBarPrivate*>(data); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	}

	explicit VariableBarPrivate(VariableBar* widget)
		:_widget(widget)
	{
		// Only link when not in design mode.
		if (!ObjectExtension::inDesigner())
		{
			_variable.setHandler(this);
		}
	}

	~VariableBarPrivate() override
	{
		// Clear the handler when destructing.
		_variable.setHandler(nullptr);
	}

	void VariableEventHandler
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
				_widget->setToolTip(QString::fromStdString(link_var.getDescription()));
				_widget->update();
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
			return QRect();
		}
		QRect r = _widget->rect();
		int ox = 0;
		int oy = 0;
		while (w != nullptr && w->isVisible() && !w->isWindow() && w->parentWidget())
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
	_private = new VariableBarPrivate(this);
//	setAttribute(Qt::WA_TransparentForMouseEvents);
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_KeyboardFocusChange);
	//setAttribute(Qt::WA_NoChildEventsForParent, true);
	//setAttribute(Qt::WA_AcceptDrops, style()->styleHint(QStyle::SH_FocusFrame_AboveWidget, 0, this));
}

bool VariableBar::isRequiredProperty(const QString& name)
{
	if (VariableWidgetBase::isRequiredProperty(name))
	{
		return true;
	}
	static const char* keys[] =
		{
			"geometry",
			"whatsThis",
			"styleSheet",
		};
	// Check if passed property name is in the keys list.
	return std::any_of(&keys[0], &keys[sizeof(keys) / sizeof(keys[0])], [name](const char* prop)
	{
		return name == prop;
	});
}

void VariableBar::applyReadOnly(bool yn)
{
	setFocusPolicy(yn ? Qt::NoFocus : Qt::StrongFocus);
}

QSize VariableBar::minimumSizeHint() const
{
	// Not sure if sizeHint() needs to be implemented.
	ensurePolished();
	QFontMetrics fm(font());
	int h = fm.height();
	int w = fm.height();
	QStyleOptionFrame opt;
	opt.initFrom(this);
	//return QSize(w, h);
	auto sz = style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h), this);
	return sz;
}

void VariableBar::paintEvent(QPaintEvent* ev)
{
	VariableWidgetBase::paintEvent(ev);
	QStyleOptionFrame so;
	so.initFrom(this);
	// Needed otherwise no focus rectangle is drawn.
	so.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &so);
	// Create painter for this widget.
	QStylePainter p(this);
	//
	//p.setRenderHint(QPainter::Antialiasing);
	// Use base function to draw the focus rectangle having the right color as a regular edit widget.
	p.drawPrimitive(QStyle::PE_PanelLineEdit, so);
	auto rc = p.style()->subElementRect(QStyle::SE_LineEditContents, &so, this);
	// Create additional spacing.
	inflate(rc, -so.lineWidth);
	// Get variable as a reference.
	auto& v(VariableBarPrivate::cast(_private)->_variable);
	// Set designer text as default.
	QString text("Value Unit");
	auto pos = rc.size().width() / 2;
	if (!inDesigner())
	{
		text = QString::fromStdString(v.getCurString() + ' ' + v.getUnit());
		pos = (int)Value::calculateOffset(v.getCur(), v.getMin(), v.getMax(), Value(rc.width()), true).getInteger();
	}
	// First part.
	p.setClipRect(QRect(rc.topLeft(), QSize(pos, rc.height())));
	p.setBrush(palette().color(QPalette::Highlight));
	p.setPen(Qt::NoPen);
	p.drawRect(rc);
	p.setClipRect(rc);
	p.setPen(QPen(palette().color(QPalette::HighlightedText)));
	p.drawText(QRectF(rc), Qt::AlignCenter, text);
	// Second part.
	p.setClipRect(QRect(QPoint(pos, 0), rc.bottomRight()));
	p.setBrush(palette().color(QPalette::Base));
	p.setPen(Qt::NoPen);
	p.drawRect(rc);
	p.setPen(QPen(palette().color(QPalette::Text)));
	p.drawText(QRectF(rc), Qt::AlignCenter, text);
}

void VariableBar::keyPressEvent(QKeyEvent* event)
{
	_private->keyPressEvent(event);
}

}