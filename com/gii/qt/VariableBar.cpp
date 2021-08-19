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
		// Only link when not in design mode.
		if (!ObjectExtension::inDesigner())
		{
			_variable.setHandler(this);
		}
		QTimer::singleShot(0, this, &VariableBar::Private::connectLabelNameAlt);
	}

	void onDestroyed(QObject *obj = nullptr) // NOLINT(readability-make-member-function-const)
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
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_KeyboardFocusChange);
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
	auto& v(Private::cast(_p)->_variable);
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
	p.drawText(rc, Qt::AlignCenter, text);
	// Second part.
	p.setClipRect(QRect(QPoint(pos, 0), rc.bottomRight()));
	p.setBrush(palette().color(QPalette::Base));
	p.setPen(Qt::NoPen);
	p.drawRect(rc);
	p.setPen(QPen(palette().color(QPalette::Text)));
	p.drawText(rc, Qt::AlignCenter, text);
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
