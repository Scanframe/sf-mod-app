#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include "../gen/ScriptInterpreter.h"
#include "../global.h"

class QTextDocument;

namespace sf
{

/**
 * @brief Highlighter of a document class used by edit widgets.
 *
 * Highlights the document according the passed script interpreter.
 */
class _MISC_CLASS ScriptHighlighter :public QSyntaxHighlighter
{
	public:
		/**
		 * @brief Constructor.
		 *
		 * @param script Script interpreter derived class.
		 * @param document Document from a edit widget for example.
		 */
		explicit ScriptHighlighter(ScriptInterpreter* script, QTextDocument* document);

	protected:
		void highlightBlock(const QString& text) override;

	private:

		void highlightCurrentLine();

		struct HighlightingRule
		{
			QRegularExpression pattern;
			QTextCharFormat format;
		};

		QVector<HighlightingRule> _rules;

		QColor _curLineColor;
};

}