#include <QtWidgets>
#include <QtNetwork>
#include <QTcpSocket>

#include <iostream>
#include <stdio.h>
#include <list>
#include <sstream>
#include <string>

#include "clientstab.h"
#include "players_window.h"


ClientGUI::ClientGUI(ClientsTab* _parent, QString _host, int _port, QString _password)
	: Client(this, _host, _port, _password), QWidget(_parent)
{
	playersWindow = NULL;
	parent = _parent;
	displayTrayWarning = true;

	setLayout(createMainLayout());

	setObjectName(_host + "_" + QString::number(_port));
	if(Client::connect())
		QTimer::singleShot(5000, this, SLOT(pingTimer()));

	requestStatusInfo();
}

void ClientGUI::requestStatusInfo()
{
	StatusInfo_t statusInfo;
	connectionError_t ret = Client::getServerData(statusInfo);

	if(ret == FAIL_CONNECT)
	{
		statusErrorLabel->setVisible(true);
		return;
	}

	if(ret != CONNECT_SUCCESS)
		return;

	internalUpdateStatusInfo(statusInfo);
	//QTimer::singleShot(30000, this, SLOT(statusInfoTimer()));
}

void ClientGUI::updateStatusInfo()
{
	StatusInfo_t statusInfo;
	connectionError_t ret = Client::getServerData(statusInfo);

	if(ret != CONNECT_SUCCESS)
	{
		if(ret == FAIL_CONNECT)
			statusErrorLabel->setVisible(true);

		return;
	}

	statusErrorLabel->setVisible(false);
	internalUpdateStatusInfo(statusInfo);
}

void ClientGUI::internalUpdateStatusInfo(StatusInfo_t statusInfo)
{
	serverInfo[SERVERINFO_STATUS]->setText("<font color=green>ONLINE</font>");
	serverInfo[SERVERINFO_SERVERNAME]->setText(statusInfo.value("servername"));
	serverInfo[SERVERINFO_UPTIME]->setText(transformUptime(statusInfo.value("uptime").toInt()));
	serverInfo[SERVERINFO_LOCATION]->setText(statusInfo.value("location"));
	serverInfo[SERVERINFO_URL]->setText(statusInfo.value("url"));

	serverInfo[SERVERINFO_OWNER]->setText(statusInfo.value("name") + "(" + statusInfo.value("email") + ")");

	serverInfo[SERVERINFO_PLAYERS]->setText(statusInfo.value("online") +
											" (" + statusInfo.value("peak") +
											") / " + statusInfo.value("peak"));

	serverInfo[SERVERINFO_SERVER]->setText(statusInfo.value("server") +
											" " + statusInfo.value("version") +
											" (" + statusInfo.value("client") + ")");

	playersButton->setEnabled(true);
}

void ClientGUI::statusInfoTimer()
{
	updateStatusInfo();
	QTimer::singleShot(30000, this, SLOT(statusInfoTimer()));
}

void ClientGUI::pingTimer()
{
	if(isConnected())
	{
		if(!ping() && displayTrayWarning)
		{
			parent->parent->getTrayIcon()->showMessage("Server offline!",
					"Server " + getHost() + ":" + QString::number(getPort()) + " is not responding.",
					QSystemTrayIcon::Warning,
					10 * 1000);
			parent->parent->lastHost = getHost();
			parent->parent->lastPort = getPort();

			setConnected(false);
			displayTrayWarning = false;
		}
	}
	else
	{
		if((Client::connect()) &&
				!displayTrayWarning)
		{
			parent->parent->getTrayIcon()->showMessage("Server online!",
					"Server " + getHost() + ":" + QString::number(getPort()) + " is responding again.",
					QSystemTrayIcon::Information,
					10 * 1000);
			parent->parent->lastHost = getHost();
			parent->parent->lastPort = getPort();

			displayTrayWarning = true;
		}
	}

	QTimer::singleShot(5000, this, SLOT(pingTimer()));
}

void ClientGUI::consoleLog(QString message)
{
	QString tmp = "[" + QTime::currentTime().toString("hh:mm:ss") + "] " + message;
	console->append(tmp);
}

QString ClientGUI::transformUptime(int uptime)
{
	int hours = uptime / 3600;
	uptime -= hours * 3600;

	int minutes = uptime / 60;

	return tr("%1h & ").arg(hours) + tr("%1m").arg(minutes);
}

void ClientGUI::playersClicked()
{
	if(!playersWindow)
		playersWindow = new PlayersWindow(this, getHost(), getPort());;

	playersWindow->show();
}

QHBoxLayout* ClientGUI::createMainLayout()
{
	/*
		CONSOLE
	*/
	console = new QTextEdit;
	console->setReadOnly(true);

	QGroupBox* clientGroupBox = new QGroupBox(tr("Console"));
	QVBoxLayout* leftPanel = new QVBoxLayout;
	leftPanel->addWidget(console);
	clientGroupBox->setLayout(leftPanel);

	/*
		Right panel
	*/
	QVBoxLayout* rightPanel = new QVBoxLayout;
	rightPanel->addWidget(createStatusGroupBox());
	rightPanel->addWidget(createLuaGroupBox());
	rightPanel->addWidget(createSqlGroupBox());
	rightPanel->setSizeConstraint(QLayout::SetFixedSize);

	/*
		Main layout
	*/
	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->addWidget(clientGroupBox);
	mainLayout->addLayout(rightPanel);

	return mainLayout;
}

QGroupBox *ClientGUI::createStatusGroupBox()
{
	QGroupBox* statusGroupBox = new QGroupBox(tr("Server status"), this);
	QFormLayout* statusLayout = new QFormLayout;


	serverInfo[SERVERINFO_STATUS] = new QLabel("<font color=red>OFFLINE</font>");
	serverInfo[SERVERINFO_SERVERNAME] = new QLabel("");
	serverInfo[SERVERINFO_UPTIME] = new QLabel("");
	serverInfo[SERVERINFO_LOCATION] = new QLabel("");
	serverInfo[SERVERINFO_URL] = new QLabel("");
	serverInfo[SERVERINFO_OWNER] = new QLabel("");
	serverInfo[SERVERINFO_PLAYERS] = new QLabel("");
	serverInfo[SERVERINFO_SERVER] = new QLabel("");

	statusLayout->addRow("Status:", serverInfo[SERVERINFO_STATUS]);
	statusLayout->addRow("Server name:", serverInfo[SERVERINFO_SERVERNAME]);
	statusLayout->addRow("Uptime:", serverInfo[SERVERINFO_UPTIME]);
	statusLayout->addRow("Location:", serverInfo[SERVERINFO_LOCATION]);
	statusLayout->addRow("Url:", serverInfo[SERVERINFO_URL]);
	statusLayout->addRow("Owner:", serverInfo[SERVERINFO_OWNER]);
	statusLayout->addRow("Players:", serverInfo[SERVERINFO_PLAYERS]);
	statusLayout->addRow("Server:", serverInfo[SERVERINFO_SERVER]);

	statusErrorLabel = new QLabel("<font color=red><b>SERVER NOT RESPOND</b></font>");
	statusLayout->addRow(statusErrorLabel);
	statusErrorLabel->setVisible(false);

	playersButton = new QPushButton("Players list");
	playersButton->setDisabled(true);
	statusLayout->addRow(playersButton);

	QObject::connect(playersButton, SIGNAL(clicked()), this, SLOT(playersClicked()));


	statusGroupBox->setLayout(statusLayout);

	statusGroupBox->setFixedWidth(300);
	statusGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	return statusGroupBox;
}

QGroupBox *ClientGUI::createLuaGroupBox()
{
	QGroupBox* luaGroupBox = new QGroupBox(tr("LUA"));

	lua = new QTextEdit("TODO");

	QVBoxLayout* luaLayout = new QVBoxLayout;
	luaLayout->addWidget(lua);

	luaGroupBox->setLayout(luaLayout);
	luaGroupBox->setEnabled(false);
	luaGroupBox->setFixedWidth(300);

	return luaGroupBox;
}

QGroupBox *ClientGUI::createSqlGroupBox()
{
	QGroupBox* sqlGroupBox = new QGroupBox(tr("SQL"));

	sql = new QTextEdit("Enter your sql queries to execute here");

	QVBoxLayout* sqlLayout = new QVBoxLayout;
	sqlLayout->addWidget(sql);

	sqlGroupBox->setLayout(sqlLayout);
	sqlGroupBox->setEnabled(false);
	sqlGroupBox->setFixedWidth(300);

	return sqlGroupBox;
}
