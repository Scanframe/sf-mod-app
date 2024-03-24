#include "FindReplaceDialog.h"
#include <QtWidgets>
#include <QPlainTextEdit>
#include <QRegularExpression>

namespace sf
{

FindReplaceDialog::FindReplaceDialog(QWidget* parent)
	:QDialog(parent)
{
	setSizeGripEnabled(true);
	lblFind = new QLabel(tr("Find"));
	leFind = new QLineEdit;
	lblFind->setBuddy(leFind);

	lblReplace = new QLabel(tr("Replace"));
	leReplaceField = new QLineEdit;
	lblReplace->setBuddy(leReplaceField);

	cbCase = new QCheckBox(tr("Match &case"));
	cbCase->setChecked(false);
	cbRegex = new QCheckBox(tr("Re&gex"));
	cbRegex->setChecked(false);

	btnFind = new QPushButton(tr("&Find"));
	connect(btnFind, &QPushButton::clicked, this, &FindReplaceDialog::find);
	btnFind->setDefault(true);

	btnReplace = new QPushButton(tr("&Replace"));
	connect(btnReplace, &QPushButton::clicked, this, &FindReplaceDialog::replace);

	btnReplaceAll = new QPushButton(tr("Replace All"));
	connect(btnReplaceAll, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);

	btnShowMore = new QPushButton(tr("&More"));
	btnShowMore->setCheckable(true);

	btnShowMore->setAutoDefault(false);

	extension = new QWidget;

	cbWholeWords = new QCheckBox(tr("&Whole words"));
	cbBackward = new QCheckBox(tr("Search &backward"));
	cbSearchSelection = new QCheckBox(tr("Search se&lection"));

	buttonBox = new QDialogButtonBox(Qt::Vertical);
	buttonBox->addButton(btnFind, QDialogButtonBox::ActionRole);
	buttonBox->addButton(btnReplace, QDialogButtonBox::ActionRole);
	buttonBox->addButton(btnReplaceAll, QDialogButtonBox::ActionRole);
	buttonBox->addButton(btnShowMore, QDialogButtonBox::ActionRole);

	connect(cbRegex, &QCheckBox::toggled, this, &FindReplaceDialog::regexMode);

	connect(btnShowMore, &QAbstractButton::toggled, [&](bool checked)
	{
		extension->setVisible(checked);
		resizeToContent();
	});

	auto extensionLayout = new QVBoxLayout;
	extensionLayout->setContentsMargins({});
	extensionLayout->addWidget(cbWholeWords);
	extensionLayout->addWidget(cbBackward);
	extensionLayout->addWidget(cbSearchSelection);
	extension->setLayout(extensionLayout);

	auto topLeftLayout = new QHBoxLayout;
	topLeftLayout->addWidget(lblFind);
	topLeftLayout->addWidget(leFind);

	auto replaceLayout = new QHBoxLayout;
	replaceLayout->addWidget(lblReplace);
	replaceLayout->addWidget(leReplaceField);

	auto leftLayout = new QVBoxLayout;
	leftLayout->addLayout(topLeftLayout);
	leftLayout->addLayout(replaceLayout);
	leftLayout->addWidget(cbCase);
	leftLayout->addWidget(cbRegex);

	auto mainLayout = new QGridLayout;
	mainLayout->addLayout(leftLayout, 0, 0);
	mainLayout->addWidget(buttonBox, 0, 1);
	mainLayout->addWidget(extension, 1, 0, 1, 2);
	mainLayout->setRowStretch(2, 1);

	setLayout(mainLayout);
	setWindowTitle(tr("Find and Replace"));

	extension->hide();
}

FindReplaceDialog::~FindReplaceDialog()
{
	(void)this;
}

void FindReplaceDialog::showEvent(QShowEvent* event)
{
	resizeToContent();
}

bool FindReplaceDialog::find()
{
	bool rv = false;
	auto query = leFind->text();
	QRegularExpression re;
	auto editor = getEditor();
	if (!editor)
	{
		return rv;
	}
	if (cbRegex->isChecked())
	{
		re = QRegularExpression(query);
		if (cbBackward->isChecked())
		{
			rv = editor->find(re, QTextDocument::FindBackward);
		}
		else
		{
			rv = editor->find(re);
		}
	}
	if (cbSearchSelection->isChecked())
	{
		query = editor->textCursor().selectedText();
		editor->find(query);
	}
	else
	{
		if (cbBackward->isChecked())
		{
			if (cbWholeWords->isChecked() and cbCase->isChecked())
			{
				rv = editor->find(query, QTextDocument::FindWholeWords | QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);
			}
			else if (cbWholeWords->isChecked() and cbCase->isChecked() != true)
			{
				rv = editor->find(query, QTextDocument::FindWholeWords | QTextDocument::FindBackward);
			}
			else if (cbCase->isChecked())
			{
				rv = editor->find(query, QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);
			}
			else
			{
				rv = editor->find(query, QTextDocument::FindBackward);
			}
			// Start from end of document when not found.
			if (!rv)
			{
				cursor = editor->textCursor();
				cursor.movePosition(QTextCursor::End);
				editor->setTextCursor(cursor);
			}
		}
		else
		{
			if (cbWholeWords->isChecked() and cbCase->isChecked())
			{
				rv = editor->find(query, QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively);
			}
			else if (cbWholeWords->isChecked() and cbCase->isChecked() != true)
			{
				rv = editor->find(query, QTextDocument::FindWholeWords);
			}
			else if (cbCase->isChecked())
			{
				rv = editor->find(query, QTextDocument::FindCaseSensitively);
			}
			else
			{
				rv = editor->find(query);
			}
			// Start from start when not found.
			if (!rv)
			{
				cursor = editor->textCursor();
				cursor.movePosition(QTextCursor::Start);
				editor->setTextCursor(cursor);
			}
		}
	}
	return rv;
}

void FindReplaceDialog::replace()
{
	QPlainTextEdit* Editor = getEditor();
	cursor = Editor->textCursor();
	if (cursor.hasSelection())
	{
		cursor.insertText(leReplaceField->text());
		Editor->setTextCursor(cursor);
	}
}

void FindReplaceDialog::replaceAll()
{
	while (find())
	{
		replace();
	}
}

void FindReplaceDialog::regexMode()
{
	cbCase->setChecked(false);
	cbWholeWords->setChecked(false);
	cbCase->setEnabled(false);
	cbWholeWords->setEnabled(false);
}

void FindReplaceDialog::resizeToContent()
{
	QTimer::singleShot(0, [&]()
	{
		resize(width(), (minimumSizeHint() - extension->minimumSizeHint()).height());
	});
}

}
