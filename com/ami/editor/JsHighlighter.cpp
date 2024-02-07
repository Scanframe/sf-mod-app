#include "JsHighlighter.h"

namespace sf
{

JsHighlighter::JsHighlighter(QTextDocument* parent, bool dark_mode)
	:QSyntaxHighlighter(parent)
{
	// The order of appending rules is important otherwise functions in commented lines are highlighted too.
	HighlightingRule rule;
	if (dark_mode)
	{
		keywordFormat.setForeground(QColor(QColorConstants::Blue).lighter());
	}
	else
	{
		keywordFormat.setForeground(QColor(QColorConstants::DarkBlue));
	}
	keywordFormat.setFontWeight(QFont::Bold);
	auto keywords = R"(
let
var
new
function
break
case
catch
const
continue
default
delete
do
else
for
function
if
in
instanceof
new
return
switch
this
throw
try
typeof
var
void
while
with
)";
	for (const QString& keyword : QString(keywords).split('\n'))
	{
		if (keyword.length())
		{
			rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(keyword));
			rule.format = keywordFormat;
			highlightingRules.append(rule);
		}
	}

	quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
	rule.format = quotationFormat;
	highlightingRules.append(rule);

	functionFormat.setFontItalic(true);
	functionFormat.setForeground(Qt::magenta);
	rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
	rule.format = functionFormat;
	highlightingRules.append(rule);

	singleLineCommentFormat.setForeground(Qt::gray);
	rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::gray);

	doubleQuotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegularExpression(QStringLiteral("'.*'"));
	rule.format = doubleQuotationFormat;
	highlightingRules.append(rule);

	commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
	commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void JsHighlighter::highlightBlock(const QString& text)
{
	for (const HighlightingRule& rule : std::as_const(highlightingRules))
	{
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext())
		{
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}
	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
	{
		startIndex = text.indexOf(commentStartExpression);
	}

	while (startIndex >= 0)
	{
		QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
		int endIndex = match.capturedStart();
		int commentLength = 0;
		if (endIndex == -1)
		{
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex + match.capturedLength();
		}
		setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
	}
}

}
