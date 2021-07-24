#include <QPlainTextEdit>
#include "ScriptHighlighter.h"

namespace sf
{

ScriptHighlighter::ScriptHighlighter(ScriptInterpreter* script, QTextDocument* document)
	:QSyntaxHighlighter(document)
{
	auto pte = qobject_cast<QPlainTextEdit*>(document->parent()->parent());
	auto dark_mode = pte ? pte->palette().color(QPalette::Base).lightness() < 127 : false;

	_curLineColor = (dark_mode ? QColorConstants::DarkYellow.darker() : QColor(255, 250, 227));
	//
	// The order of appending rules is important otherwise functions in commented lines are highlighted too.
	//
	// Identifiers.
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Blue.lighter() : QColorConstants::DarkBlue);
		rule.format.setFontWeight(QFont::Bold);
		for (auto& id: script->getIdentifiers(ScriptObject::idKeyword))
		{
			rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(id.c_str()));
			_rules.append(rule);
		}
	}
	// Constants.
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Cyan : QColorConstants::DarkCyan);
		rule.format.setFontWeight(QFont::Bold);
		for (auto& id: script->getIdentifiers(ScriptObject::idConstant))
		{
			rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(id.c_str()));
			_rules.append(rule);
		}
	}
	// Punctuators.
	{
		HighlightingRule rule;
		rule.format.setFontWeight(QFont::Bold);
		rule.pattern = QRegularExpression(QStringLiteral("[\\[\\](){},:;]"));
		_rules.append(rule);
	}
	// Operators.
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Blue : QColorConstants::DarkBlue);
		rule.format.setFontWeight(QFont::Bold);
		rule.pattern = QRegularExpression(QStringLiteral("[=+-/|%^*]"));
		_rules.append(rule);
	}
	// Number values.
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Red.lighter() : QColorConstants::DarkRed);
		rule.pattern = QRegularExpression(QStringLiteral("[+-]?(?<![a-zA-Z_])([0-9]*[.])?[0-9]+([eE][+-]?[0-9]*)?"));
		_rules.append(rule);
	}
	// Type definitions.
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Blue.lighter() : QColorConstants::DarkBlue);
		rule.format.setFontItalic(true);
		for (auto& id: script->getIdentifiers(ScriptObject::idTypedef))
		{
			rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(id.c_str()));
			_rules.append(rule);
		}
	}
	// Labels
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Magenta : QColorConstants::DarkMagenta);
		rule.format.setFontWeight(QFont::Bold);
		rule.pattern = QRegularExpression(QStringLiteral("(?<=:)\\b([A-Za-z0-9_]+)(?=;)"));
		_rules.append(rule);
	}
	// Functions: None global
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Magenta : QColorConstants::DarkMagenta);
		rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
		_rules.append(rule);
	}
	// Functions: Global registered.
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Magenta : QColorConstants::DarkMagenta);
		rule.format.setFontItalic(true);
		for (auto& keyword: script->getIdentifiers(ScriptObject::idFunction))
		{
			rule.pattern = QRegularExpression(QStringLiteral("\\b%1\\b(?=\\()").arg(keyword.c_str()));
			_rules.append(rule);
		}
	}
	// Single line comment.
	{
		HighlightingRule rule;
		rule.format.setForeground(QColorConstants::Gray);
		rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
		_rules.append(rule);
	}
	// Double quoted strings.
	{
		HighlightingRule rule;
		rule.format.setForeground(dark_mode ? QColorConstants::Green : QColorConstants::DarkGreen);
		rule.pattern = QRegularExpression(QStringLiteral("\".*?(?<!\\\\)\""));
		_rules.append(rule);
	}
	if (pte)
	{
		connect(pte, &QPlainTextEdit::cursorPositionChanged, this, &ScriptHighlighter::highlightCurrentLine);
		highlightCurrentLine();
	}
}

void ScriptHighlighter::highlightCurrentLine()
{
	if (auto pte = qobject_cast<QPlainTextEdit*>(parent()->parent()->parent()))
	{
		QList<QTextEdit::ExtraSelection> extraSelections;

		if (!pte->isReadOnly())
		{
			QTextEdit::ExtraSelection selection;
			selection.format.setBackground(_curLineColor);
			selection.format.setProperty(QTextFormat::FullWidthSelection, true);
			selection.cursor = pte->textCursor();
			selection.cursor.clearSelection();
			extraSelections.append(selection);
		}
		pte->setExtraSelections(extraSelections);
	}
}

void ScriptHighlighter::highlightBlock(const QString& text)
{
	for (const HighlightingRule& rule : qAsConst(_rules))
	{
		auto matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext())
		{
			auto match = matchIterator.next();
			setFormat((int) match.capturedStart(), (int) match.capturedLength(), rule.format);
		}
	}
}

}
