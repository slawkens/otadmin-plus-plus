#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>

#include "highlighter.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT

	public:
		CodeEditor(QWidget *parent = 0);

		void lineNumberAreaPaintEvent(QPaintEvent *event);
		int lineNumberAreaWidth();

		int getErrorLine() const;
		void setErrorLine(int line);

		int getCurrentLine() const;
		void setCurrentLine(int line);

	protected:
		void resizeEvent(QResizeEvent *event);
		void contextMenuEvent(QContextMenuEvent *event);

	private slots:
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect &, int);

		void contextMenuAction(QAction *action);

	//	void insertActions(

	 private:
		QWidget *lineNumberArea;
		Highlighter *highlighter;
};


class LineNumberArea : public QWidget
{
	public:
		LineNumberArea(CodeEditor *editor) : QWidget(editor) {
			errorLine = currentLine = -1;
			codeEditor = editor;
		}

		QSize sizeHint() const {
			return QSize(codeEditor->lineNumberAreaWidth(), 0);
		}

		int errorLine, currentLine;

	protected:
		void paintEvent(QPaintEvent *event) {
			codeEditor->lineNumberAreaPaintEvent(event);
		}

	private:
		CodeEditor *codeEditor;
};
#endif
