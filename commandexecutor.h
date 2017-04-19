#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include <QtWidgets>
#include <QDialog>

#include "client.h"
#include "codeeditor.h"

class QLabel;
class QComboBox;
class QLineEdit;
class QToolButton;
class QSpinBox;
class QGroupBox;

enum executeError_t
{
	ERROR_NOTCONNECTED,
	ERROR_ALREADY_CONNECTED,
	ERROR_SERVER_NOT_SET,
	ERROR_COMMAND,
	ERROR_COMMAND_FAILED,
	ERROR_PARAMETERS,
	COMMAND_OK
};

class CommandExecutor : public QDialog
{
	Q_OBJECT

	public:
		CommandExecutor(QWidget *_parent = 0);
		void loadFiles();

		executeError_t execute(QString commandLine);
		void internalStartExecuting();

		void pause();
		void setRunning(bool _running);
		void errorNotice(QString error);

	public slots:
		void loadClicked();
		void executeClicked();

	private slots:
		void executeTimerTimeout();
		void repeatTimerTimeout();

	protected:
		void closeEvent(QCloseEvent* event);

	private:
		void createMainLayout();
		QGroupBox *createCommandListGroupBox();
		QGroupBox *createFilesGroupBox();
		QGroupBox *createRepeatGroupBox();

		QWidget *parent;
		CodeEditor *commandEdit;
		QTimer *timer, *repeatTimer;

		QGroupBox *repeatGroupBox;
		QLabel *repeatTimerLabel;
		QCheckBox *repeatIgnoreFirst;
		QSpinBox *repeatSpinBox, *repeatLimitSpinBox;

		QPushButton *executeButton, *loadButton, *saveButton;
		QComboBox *filesComboBox;
		QStringList commands;

		Client* client;
		int line, repeats;
		bool running;
};

#endif
