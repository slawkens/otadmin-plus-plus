#include <QtWidgets>

#include "codeeditor.h"
#include "highlighter.h"

CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit(parent)
{
/*	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);
	setFont(font);
*/
	highlighter = new Highlighter(document());
	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(updateLineNumberArea(const QRect &, int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

int CodeEditor::getErrorLine() const
{
	if(LineNumberArea* tmp = (LineNumberArea*)lineNumberArea)
		return tmp->errorLine;

	return -1;
}

void CodeEditor::setErrorLine(int line)
{
	if(LineNumberArea* tmp = (LineNumberArea*)lineNumberArea)
		tmp->errorLine = line - 1;
}

int CodeEditor::getCurrentLine() const
{
	if(LineNumberArea* tmp = (LineNumberArea*)lineNumberArea)
		return tmp->currentLine;

	return -1;
}

void CodeEditor::setCurrentLine(int line)
{
	if(LineNumberArea* tmp = (LineNumberArea*)lineNumberArea)
		tmp->currentLine = line - 1;
}

int CodeEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10)
	{
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	int errorLine = getErrorLine();
	int currentLine = getCurrentLine();

	painter.setPen(Qt::black);
	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			//painter.setPen(Qt::black);

			if(errorLine == blockNumber || currentLine == blockNumber)
			{
				QColor color;
				if(errorLine == blockNumber)
					color = Qt::red;
				else
					color = Qt::green;

				painter.fillRect(0, top, lineNumberArea->width(), fontMetrics().height(), color);
			}

			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
					Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

void CodeEditor::contextMenuAction(QAction *action)
{
	QString toInsert = action->text();
	if(!toPlainText().split("\n").at(textCursor().blockNumber()).trimmed().isEmpty())
		toInsert = "\n" + toInsert;

	insertPlainText(toInsert);
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event)
{
	QStringList list;
	list << "server localhost 7171"
			<< "connect test"
			<< "broadcast message"
			<< "kickplayer name"
			<< "setowner houseId, ownerName"
			<< "openserver"
			<< "closeserver"
			<< "payhouses"
			<< "saveserver"
			<< "shutdown"
			<< "sleep timeInMs"
			<< "disconnect";

	QMenu *menu = createStandardContextMenu();

	QMenu *commandsMenu = new QMenu;
	commandsMenu->setTitle("Commands");
	//commandsMenu->addActions(tr("connect localhost 7171"));
	QString str;
	foreach(str, list)
		commandsMenu->addAction(str);

	connect(commandsMenu, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuAction(QAction*)));

	menu->addMenu(commandsMenu);
	menu->exec(event->globalPos());

	delete commandsMenu;
	delete menu;
}
