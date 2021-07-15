#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;

namespace sf
{

class Highlighter :public QSyntaxHighlighter
{
	Q_OBJECT

	public:
		explicit Highlighter(QTextDocument* parent = nullptr, bool dark_mode = false);

	protected:
		void highlightBlock(const QString& text) override;

	private:
		struct HighlightingRule
		{
			QRegularExpression pattern;
			QTextCharFormat format;
		};
		QVector<HighlightingRule> highlightingRules;

		QRegularExpression commentStartExpression;
		QRegularExpression commentEndExpression;

		QTextCharFormat keywordFormat;
		QTextCharFormat singleLineCommentFormat;
		QTextCharFormat multiLineCommentFormat;
		QTextCharFormat doubleQuotationFormat;
		QTextCharFormat quotationFormat;
		QTextCharFormat functionFormat;
};

}
