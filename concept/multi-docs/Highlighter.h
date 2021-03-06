#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;

QT_END_NAMESPACE

class Highlighter :public QSyntaxHighlighter
{
	Q_OBJECT

	public:
		explicit Highlighter(QTextDocument* parent = nullptr);

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
