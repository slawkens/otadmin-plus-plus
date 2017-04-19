#include <QtWidgets>

#include "commandexecutor.h"
#include "codeeditor.h"


CommandExecutor::CommandExecutor(QWidget* _parent)
	: QDialog(_parent)
{
	parent = _parent;
	client = NULL, line = repeats = 0, running = false;


	createMainLayout();
	loadFiles();


	timer = new QTimer(this), repeatTimer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(executeTimerTimeout()));
	connect(repeatTimer, SIGNAL(timeout()), this, SLOT(repeatTimerTimeout()));

	connect(executeButton, SIGNAL(clicked()), this, SLOT(executeClicked()));

	setAttribute(Qt::WA_DeleteOnClose);
	resize(550, 300);
	setWindowFlags(Qt::Window);
	setWindowTitle("Commands - " + qApp->applicationName());
	show();
}

void CommandExecutor::setRunning(bool _running)
{
	line = 0;
	running = _running;

	commandEdit->setDisabled(_running);
	if(_running)
	{
		executeButton->setText("Pause");
		executeButton->setToolTip("Terminate current script");
	}
	else
	{
		executeButton->setText("Execute");
		executeButton->setToolTip("Execute commands");
	}

	commandEdit->setErrorLine(0);
	commandEdit->setCurrentLine(0);

	repeatGroupBox->setDisabled(_running);

	if(running)
	{
		QList<QTextEdit::ExtraSelection> extraSelections;
		commandEdit->setExtraSelections(extraSelections);
	}
}

void CommandExecutor::errorNotice(QString error)
{
	int _line = line;
	setRunning(false);
	commandEdit->setErrorLine(_line);
	QMessageBox::warning(this, "Error", "<b>Error!</b><br>An error occured while executing commands.<br><br>Error description: <br>Line: <b>" + QString::number(_line) + "</b><br>" + error);
}

void CommandExecutor::closeEvent(QCloseEvent* event)
{
	if(running)
	{
		bool close = true;

		QMessageBox msgBox;
		msgBox.setWindowTitle("Confirm");
		msgBox.setText("Are you sure you want to stop executing current script?");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();
		if(ret == QMessageBox::Cancel)
			close = false;

		if(!close)
			event->ignore();
	}
}

void CommandExecutor::loadClicked()
{
	QString fileStr = filesComboBox->currentText();
	if(fileStr.isEmpty()) return;

	QFile f(QDir::currentPath() + "/scripts/" + fileStr);
	if(!f.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, "Error", "Can't open selected script! (Error: " + f.errorString() + ")");
		return;
	}
/*
	QString str;
	QTextStream t(&f);
	while(!t.atEnd())
		str += t.readLine() + "\n";
*/
	commandEdit->setPlainText(f.readAll());
	f.close();
}

void CommandExecutor::pause()
{
	if(timer->isActive() || repeatTimer->isActive())
	{
		if(timer->isActive())
			timer->stop();

		if(repeatTimer->isActive())
			repeatTimer->stop();

		setRunning(false);
		//QMessageBox::information(this, "Paused", "Script has been terminated.");
	}
}

void CommandExecutor::executeClicked()
{
	if(running) { // wanna pause
		pause();
	}
	else
	{
		repeats = 0;
		internalStartExecuting();

		executeTimerTimeout();
	}
}

void CommandExecutor::internalStartExecuting()
{
	commandEdit->setErrorLine(0);
	commandEdit->setCurrentLine(0);
	line = 0;

	QString commandList = commandEdit->toPlainText().trimmed();
	if(commandList.isEmpty())
	{
		commandEdit->setFocus();
		return;
	}

	commands = commandList.split("\n");//, QString::SkipEmptyParts);
	//commands = commandEdit->toPlainText().split(QRegExp("\\n"));

	setRunning(true);
}

void CommandExecutor::repeatTimerTimeout()
{
	repeats++;
	internalStartExecuting();
	executeTimerTimeout();
}

void CommandExecutor::executeTimerTimeout()
{
	if(timer->isActive())
		timer->stop();
	if(repeatTimer->isActive())
		repeatTimer->stop();

	if(commands.isEmpty())
	{
		int repeatLimit = repeatLimitSpinBox->value();
		if(repeatIgnoreFirst->isChecked()) repeatLimit++;

		if(repeatGroupBox->isChecked() && repeatLimit != 1 && (repeatLimit == 0 || repeats < repeatLimit - 1))
		{
			repeatTimer->start(repeatSpinBox->value() * 1000);
			repeatTimerLabel->setText("Last executed: " + QTime::currentTime().toString("hh:mm:ss"));
			return;
		}

		setRunning(false);

		commandEdit->setCurrentLine(0);
		QMessageBox::information(this, "Finished", "Commands has been succesfully executed!");
		return;
	}

	if(repeats == 0 && repeatGroupBox->isChecked() && repeatIgnoreFirst->isChecked())
	{
		repeatTimer->start(repeatSpinBox->value() * 1000);
		return;
	}

	//qDebug() << "LINE = " << line;
	line++;
	commandEdit->setCurrentLine(line);
	commandEdit->update();

	QString command = commands.takeFirst();
	QStringList tmp = command.split(" ");
	if(tmp.at(0).toLower() == "sleep")
	{
		int time = tmp.at(1).toInt();
		if(time > 0)
			timer->start(time);
		else
			errorNotice("Unknown parameters for <i>sleep</i> command (Time must be specified in miliseconds, fe. <b>sleep 5000</b> (5 seconds))");

		return;
	}

	executeError_t ret = execute(command);
	switch(ret)
	{
		case COMMAND_OK:
			executeTimerTimeout();
			break;

		case ERROR_NOTCONNECTED:
			errorNotice("Not connected to any server!");
			break;

		case ERROR_ALREADY_CONNECTED:
			errorNotice("Already connected to server! Disconnect before connecting new server!");
			break;

		case ERROR_SERVER_NOT_SET:
			errorNotice("Server not set!");
			break;

		case ERROR_COMMAND:
			errorNotice("Unknown server command!");
			break;

		case ERROR_COMMAND_FAILED:
			errorNotice("Cannot execute command. Please try again. Be sure that login details are valid and remote server is online!");
			break;

		case ERROR_PARAMETERS:
			errorNotice("Invalid parameters used!");
			break;
	}
}

executeError_t CommandExecutor::execute(QString commandLine)
{
	commandLine = commandLine.trimmed();
	if(commandLine.isEmpty() || commandLine.at(0) == '#') //empty line or comment
		return COMMAND_OK;

	QStringList cmd = commandLine.split(" ");

	QString commandName = cmd.at(0).toLower();
	cmd.removeFirst();

	QString params = cmd.join(" ");
	if(commandName == "server")
	{
		QString host, portStr;

		QStringList tmp = params.trimmed().split(" ");
		if(tmp.size() == 2)
		{
			host = tmp.at(0);
			portStr = tmp.at(1);
		}
		else
		{
			tmp = params.split(":");
			if(tmp.size() == 2)
			{
				host = tmp.at(0);
				portStr = tmp.at(1);
			}
		}

		host = host.trimmed();
		int port = portStr.trimmed().toInt();
		if(host.isEmpty() || port == 0)
			return ERROR_PARAMETERS;

		client = new Client(NULL, host, port);
		return COMMAND_OK;
	}
	else if(commandName == "connect")
	{
		if(client && client->isConnected())
			return ERROR_ALREADY_CONNECTED;

		if(!client)
			return ERROR_SERVER_NOT_SET;

		params = params.trimmed();
		if(params.isEmpty())
			return ERROR_PARAMETERS;

		client->setPassword(params);
		if(!client->connect())
			return ERROR_COMMAND_FAILED;

		return COMMAND_OK;
	}
	else if(commandName == "disconnect")
	{
		if(!client || !client->isConnected())
			return ERROR_NOTCONNECTED;

		client->disconnect();
		delete client;
		return COMMAND_OK;
	}


	QStringList commands;
	commands << "broadcast" << "kickplayer" << "setowner"
			<< "openserver" << "closeserver" << "payhouses"
			<< "saveserver" << "shutdown";

	if(!commands.contains(commandName))
		return ERROR_COMMAND;

	if(!client || !client->isConnected())
		return ERROR_NOTCONNECTED;


	if(commandName == "broadcast")
	{
		params = params.trimmed();
		if(params.isEmpty())
			return ERROR_PARAMETERS;

		client->commandBroadcast(params);
	}
	else if(commandName == "kickplayer")
	{
		params = params.trimmed();
		if(params.isEmpty())
			return ERROR_PARAMETERS;

		client->commandKick(params);
	}
	else if(commandName == "setowner")
	{
		int houseId = 0;
		QString owner;

		QStringList tmp = params.trimmed().split(",");
		if(tmp.size() == 2)
		{
			houseId = tmp.at(0).toInt();
			owner = tmp.at(1).trimmed();
		}
		else
			return ERROR_PARAMETERS;

		if(owner.isEmpty() || houseId == 0)
			return ERROR_PARAMETERS;

		client->commandSetOwner(houseId, owner);
	}
	else if(commandName == "openserver")
		client->commandOpenServer();
	else if(commandName == "closeserver")
		client->commandCloseServer();
	else if(commandName == "payhouses")
		client->commandPayHouses();
	else if(commandName == "saveserver")
		client->commandSaveServer();
	else if(commandName == "shutdown")
		client->commandShutdown();

	return COMMAND_OK;
}

void CommandExecutor::createMainLayout()
{
	commandEdit = new CodeEditor(this);
	commandEdit->setPlainText("# Setup our server"
							"\nserver localhost 7171" \
							"\n\n# Connect to our server ;)" \
							"\nconnect test" \
							"\n\n# Broadcast something" \
							"\nbroadcast Hello players! :)" \
							"\n\n# wait 5 seconds" \
							"\nsleep 5000" \
							"\nbroadcast Byebye" \
							"\n\n# Remember always to disconnect from our server!" \
							"\ndisconnect");

	executeButton = new QPushButton("Execute");
	executeButton->setToolTip("Execute commands");
	executeButton->setAutoDefault(false);
	//executeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QVBoxLayout *leftPanel = new QVBoxLayout;
	leftPanel->addWidget(createCommandListGroupBox());
	leftPanel->addWidget(createFilesGroupBox());
	leftPanel->addWidget(createRepeatGroupBox());
	leftPanel->setSizeConstraint(QLayout::SetFixedSize);


	QGridLayout* mainLayout = new QGridLayout;
	mainLayout->addLayout(leftPanel, 0, 0);
	mainLayout->addWidget(commandEdit, 0, 1);
	mainLayout->addWidget(executeButton, 1, 0, 1, 2);
	setLayout(mainLayout);
}

QGroupBox *CommandExecutor::createCommandListGroupBox()
{
	QGroupBox *commandListGroupBox = new QGroupBox("Available commands", this);

	QLabel *commandsLabel = new QLabel;
	commandsLabel->setText("server <b>localhost 7171</b>" \
						"<br>connect <b>test</b>" \
						"<br>broadcast <b>message</b>" \
						"<br>kickplayer <b>name</b>" \
						"<br>setowner <b>houseId, ownerName</b>" \
						"<br>openserver" \
						"<br>closeserver" \
						"<br>payhouses" \
						"<br>saveserver" \
						"<br>shutdown" \
						"<br>sleep <b>timeInMs</b>" \
						"<br>disconnect");

	QGridLayout *commandListLayout = new QGridLayout;
	commandListLayout->setAlignment(Qt::AlignTop);
	commandListLayout->addWidget(commandsLabel);
	commandListGroupBox->setLayout(commandListLayout);

	commandListGroupBox->setFixedWidth(250);
	commandListGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	return commandListGroupBox;
}

QGroupBox *CommandExecutor::createFilesGroupBox()
{
	QGroupBox *filesGroupBox = new QGroupBox("Load script");

	QLineEdit *pathEdit = new QLineEdit(QDir::currentPath());
	pathEdit->setReadOnly(true);

	filesComboBox = new QComboBox;

	loadButton = new QPushButton("Load");
	loadButton->setAutoDefault(false);
	loadButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(loadButton);

	QVBoxLayout *filesLayout = new QVBoxLayout;
	filesLayout->addWidget(pathEdit);
	filesLayout->addWidget(filesComboBox);
	filesLayout->addLayout(buttonLayout);
	filesGroupBox->setLayout(filesLayout);

	filesGroupBox->setFixedWidth(250);
	filesGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadClicked()));

	return filesGroupBox;
}

QGroupBox *CommandExecutor::createRepeatGroupBox()
{
	repeatGroupBox = new QGroupBox("Repeat script execution");
	repeatGroupBox->setCheckable(true);
	repeatGroupBox->setChecked(false);

	repeatSpinBox = new QSpinBox;
	repeatSpinBox->setSuffix(" seconds");
	repeatSpinBox->setMinimum(1);
	repeatSpinBox->setValue(60);

	repeatLimitSpinBox = new QSpinBox;
	repeatLimitSpinBox->setSuffix(" repeats");
	repeatLimitSpinBox->setValue(0);

	repeatIgnoreFirst = new QCheckBox("Ignore first");
	repeatIgnoreFirst->setToolTip("Check if you want to ignore first execution. (So first will be executed after specified time)");

	repeatTimerLabel = new QLabel;

	QFormLayout *repeatLayout = new QFormLayout;
	repeatLayout->addRow(tr("&Repeat every:"), repeatSpinBox);
	repeatLayout->addRow(tr("&Limit: (0 to no limit) "), repeatLimitSpinBox);
	repeatLayout->addRow(repeatIgnoreFirst);
	repeatLayout->addRow(repeatTimerLabel);
	repeatGroupBox->setLayout(repeatLayout);

	repeatGroupBox->setFixedWidth(250);
	repeatGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	return repeatGroupBox;
}

void CommandExecutor::loadFiles()
{
	QDir dir(QDir::currentPath());
	if(!dir.exists())
		return;

	if(!dir.cd("scripts"))
		return;

	QStringList filters;
	filters << "*.txt";
	dir.setNameFilters(filters);

	QStringList files = dir.entryList(QDir::Files);
	QString file;
	foreach(file, files)
		filesComboBox->addItem(file);
}
