#include <QtWidgets>

#include "mainwindow.h"
#include "aboutwindow.h"
#include "definitions.h"
#include "networkmessage.h"
#include "clientstab.h"
#include "commandexecutor.h"
#include "favorites_window.h"


MainWindow::MainWindow()
{
	lastHost = "", lastPort = 0;

	createMenus();
	createTrayMenus();

	openedWindows[OPENED_WINDOW_FAVORITES] = NULL;

	clientsTab = new ClientsTab(this);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(createConnectGroupBox());
	mainLayout->addWidget(clientsTab);
	mainLayout->setMargin(2);

	setLayout(mainLayout);
	QWidget* mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);

	setCentralWidget(mainWidget);

    setMinimumSize(500, 150);
	setWindowTitle(qApp->applicationName());

	statusBar()->showMessage(qApp->applicationName() + " v" + qApp->applicationVersion());
	readSettings();
}

void MainWindow::createMenus()
{
	mainMenu = new QMenu("&Main", this);
	//serverMenu = new QMenu("Server", this);
	stylesMenu = new QMenu("Style", this);
	helpMenu = new QMenu("About", this);

	//Favorites
	//favoritesAct = new QAction("Favorites", this);
	//favoritesAct->setStatusTip("Favorites servers list");
	//favoritesAct->setShortcut(Qt::CTRL + Qt::Key_F);

	//Commands
	commandsAct = new QAction("Commands", this);
	commandsAct->setStatusTip(qApp->applicationName() + " commands executor");
	commandsAct->setShortcut(Qt::CTRL + Qt::Key_A | Qt::Key_C);
	//commandsAct->setShortcut("Ctrl+A+C");

	//Exit
	exitAct = new QAction("Exit", this);
	exitAct->setStatusTip("Exit");
	exitAct->setShortcut(Qt::ALT + Qt::Key_F4);

	//Styles
	QActionGroup *stylesActGroup = new QActionGroup(this);
	QString styles[] = {"WindowsXP", "Plastique", "Cleanlooks"};
	for(int i = 0; i <= 2; i++)
	{
		styleActs[i] = new QAction(styles[i], this);
		styleActs[i]->setCheckable(true);
		stylesActGroup->addAction(styleActs[i]);
		stylesMenu->addAction(styleActs[i]);
		styleActs[i]->setStatusTip("Change style");
	}

	//Website
	websiteAct = new QAction("&Website", this);
	websiteAct->setStatusTip("Goto " + qApp->applicationName() + " website");

	//about
	aboutAct = new QAction("&About..", this);
	aboutAct->setShortcut(Qt::CTRL + Qt::Key_H);
	aboutAct->setStatusTip("About " + qApp->applicationName());

	mainMenu->addAction(commandsAct);
	//mainMenu->addAction(favoritesAct);
	mainMenu->addSeparator();
	mainMenu->addAction(exitAct);

	helpMenu->addAction(websiteAct);
	helpMenu->addSeparator();
	helpMenu->addAction(aboutAct);

	connect(commandsAct, SIGNAL(triggered()), this, SLOT(commandExecutor()));
	//connect(favoritesAct, SIGNAL(triggered()), this, SLOT(favoritesClicked()));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(exitApp()));
	connect(websiteAct, SIGNAL(triggered()), this, SLOT(launchWebsite()));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	connect(stylesMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeStyle(QAction*)));

	//Add menu
	menuBar()->addMenu(mainMenu);
	//menuBar()->addMenu(serverMenu);
	menuBar()->addMenu(stylesMenu);
	menuBar()->addMenu(helpMenu);
}

void MainWindow::handleMessage(const QString& message)
{
	enum Action {
	Nothing,
	Open
	} action;

	action = Nothing;
	if(!message.isEmpty())
		action = Open;

	if (action == Nothing)
	{
		showNormal();
		//emit needToShow();
		return;
	}

	//TODO
	//open/execute .ota script file
}
ClientGUI* MainWindow::getCurrentClient()
{
	if(ClientGUI* tmp = clientsTab->getCurrentClient())
		return tmp;

	return NULL;
}

ClientGUI *MainWindow::findClient(QString host, int port)
{
	QString tmpName = host + "_" + QString::number(port);
	QWidget* client = clientsTab->findChild<QWidget*>(tmpName);
	if(!client)
		return NULL;

	return (ClientGUI*)client;
}

void MainWindow::connectClicked()
{
    QMessageBox::information(this, "PATH TEST", QString::number(this->size().rheight()));

	QString host = hostEdit->text();
	int port = portEdit->value();
	QString password = passwordEdit->text();

	bool hostEmpty = host.isEmpty(), portEmpty = port == 0, passwordEmpty = password.isEmpty();
	if(hostEmpty || portEmpty || passwordEmpty)
	{
		QMessageBox::warning(this, "Information", "Please fill all required inputs!");

		if(hostEmpty)
			hostEdit->setFocus();
		else if(portEmpty)
			portEdit->setFocus();
		else if(passwordEmpty)
			passwordEdit->setFocus();

		return;
	}

	ClientGUI *client = findClient(host, port);
	//QString tmpName = host + "_" + QString::number(port);
	//QWidget* tmp = clientsTab->findChild<QWidget*>(tmpName);
	if(!client)
	{
		/*ClientGUI* */client = new ClientGUI(clientsTab, host, port, password);
		int i = clientsTab->addTab(client, host + ":" + QString::number(port));
		clientsTab->setTabIcon(i, clientsTab->m_connectedIcon[client->isConnected()]);
		clientsTab->setCurrentIndex(i);
		return;
	}

	clientsTab->setCurrentWidget(client);
}
/*
void MainWindow::favoritesClicked()
{
	QWidget *favoritesWindow = openedWindows[OPENED_WINDOW_FAVORITES];
	if(!favoritesWindow)
	{
		favoritesWindow = new FavoritesWindow(this);
		openedWindows[OPENED_WINDOW_FAVORITES] = favoritesWindow;
	}
	else
	{
		if(!favoritesWindow->isVisible())
		{
			favoritesWindow->activate();
			return;
		}
	}/

	favoritesWindow->show();
}*/

void MainWindow::changeStyle(QAction* act)
{
	changeStyle(act->text());
}

void MainWindow::changeStyle(QString style)
{
	QApplication::setStyle (QStyleFactory::create (style));
	currentStyle = style;
}

void MainWindow::notImplemented()
{
	return notImplemented(this);
}

void MainWindow::notImplemented(QWidget* window)
{
	QMessageBox::information(window, "Information", "This function is _not_ implemented yet.", "Ok :(");
}

void MainWindow::exitApp()
{
	trayIcon->setVisible(false);
	close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if(displayTrayNotify)
	{
		trayIcon->showMessage(qApp->applicationName() + " is still running",
							"If you want to close it - just right click on this icon, and from menu choose 'Quit'",
							QSystemTrayIcon::MessageIcon(1),
							3 * 1000);
		displayTrayNotify = false;
	}

	trayMenuActions[TRAY_ACTION_MINIMIZE]->setEnabled(false);
	trayMenuActions[TRAY_ACTION_MAXIMIZE]->setEnabled(true);
	trayMenuActions[TRAY_ACTION_RESTORE]->setEnabled(true);

	writeSettings();
	if(trayIcon->isVisible())
	{
		hide();
		event->ignore();
	}
	else
	{
		event->accept();
		qApp->exit();
	}
}

void MainWindow::commandExecutor()
{
	CommandExecutor* cmd = new CommandExecutor(this);
	if(!cmd) return;

	//cmd->exec();
}

void MainWindow::launchWebsite()
{
    //#ifdef WIN32
    //ShellExecute(winId(), L"open", L"http://otadmin.otsoft.org", NULL, NULL, SW_SHOW);
    //#endif
    QDesktopServices::openUrl(QUrl("http://otadmin.otsoft.org", QUrl::TolerantMode));
}

void MainWindow::about()
{
	new AboutWindow(this);
}

void MainWindow::createTrayMenus()
{
	////////////////////////////////////////
	// Server commands
	////////////////////////////////////////
	trayMenuActions[TRAY_ACTION_BROADCAST] = new QAction(tr("&Broadcast message"), this);
	trayMenuActions[TRAY_ACTION_BROADCAST]->setDisabled(true);

	trayMenuActions[TRAY_ACTION_CLOSE] = new QAction(tr("&Close server"), this);
	trayMenuActions[TRAY_ACTION_CLOSE]->setDisabled(true);

	trayMenuActions[TRAY_ACTION_OPEN] = new QAction(tr("&Open server"), this);
	trayMenuActions[TRAY_ACTION_OPEN]->setDisabled(true);

	trayMenuActions[TRAY_ACTION_PAYHOUSES] = new QAction(tr("Pay &houses"), this);
	trayMenuActions[TRAY_ACTION_PAYHOUSES]->setDisabled(true);

	trayMenuActions[TRAY_ACTION_SHUTDOWN] = new QAction(tr("&Shutdown"), this);
	trayMenuActions[TRAY_ACTION_SHUTDOWN]->setDisabled(true);

	trayMenuActions[TRAY_ACTION_KICK] = new QAction(tr("&Kick player"), this);
	trayMenuActions[TRAY_ACTION_KICK]->setDisabled(true);

	trayMenuActions[TRAY_ACTION_SETOWNER] = new QAction(tr("Set house &owner"), this);
	trayMenuActions[TRAY_ACTION_SETOWNER]->setDisabled(true);

	for(int i = TRAY_ACTION_BROADCAST; i <= TRAY_ACTION_SETOWNER; i++)
		serverCommandsAct.append(trayMenuActions[i]);

	////////////////////////////////////////

	////////////////////////////////////////
	// MIMIZE | MAXIMIZE | RESTORE | QUIT
	////////////////////////////////////////
	trayMenuActions[TRAY_ACTION_MINIMIZE] = new QAction(tr("Mi&nimize"), this);
//	connect(trayMenuActions[TRAY_ACTION_MINIMIZE], SIGNAL(triggered()), this, SLOT(hide()));

	trayMenuActions[TRAY_ACTION_MAXIMIZE] = new QAction(tr("Ma&ximize"), this);
//	connect(trayMenuActions[TRAY_ACTION_MAXIMIZE], SIGNAL(triggered()), this, SLOT(showMaximized()));

	trayMenuActions[TRAY_ACTION_RESTORE] = new QAction(tr("&Restore"), this);
//	connect(trayMenuActions[TRAY_ACTION_RESTORE], SIGNAL(triggered()), this, SLOT(showNormal()));

	trayMenuActions[TRAY_ACTION_QUIT] = new QAction(tr("&Quit"), this);
	connect(trayMenuActions[TRAY_ACTION_QUIT], SIGNAL(triggered()), qApp, SLOT(quit()));
	////////////////////////////////////////

	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_BROADCAST]);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_KICK]);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_SETOWNER]);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_CLOSE]);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_OPEN]);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_PAYHOUSES]);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_SHUTDOWN]);

	trayIconMenu->addSeparator();
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_MINIMIZE]);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_MAXIMIZE]);
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_RESTORE]);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(trayMenuActions[TRAY_ACTION_QUIT]);

	connect(trayIconMenu, SIGNAL(triggered(QAction*)), this, SLOT(trayIconMenuTriggered(QAction*)));
	trayIcon = new QSystemTrayIcon(QIcon(":/images/otadmin.png"), this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setToolTip(qApp->applicationName() + " v" + qApp->applicationVersion());
	trayIcon->show();

	//tray events
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(trayMessageClicked()));
}

void MainWindow::trayIconMenuTriggered(QAction* action)
{
	bool minimized = false, maximized = false, restored = false;

	if(action == trayMenuActions[TRAY_ACTION_MINIMIZE])
	{
		hide();
		minimized = true;
	}
	else if(action == trayMenuActions[TRAY_ACTION_MAXIMIZE])
	{
		showMaximized();
		maximized = true;
	}
	else if(action == trayMenuActions[TRAY_ACTION_RESTORE])
	{
		showNormal();
		restored = true;
	}
	else if(action == trayMenuActions[TRAY_ACTION_BROADCAST])
	{
		bool ok;
		QString message = QInputDialog::getText(clientsTab, tr("Message"),
		tr("Message to broadcast"), QLineEdit::Normal, "", &ok);

		if(!ok)
			return;

		if(ClientGUI* client = getCurrentClient())
			client->commandBroadcast(message);
	}
	else if(action == trayMenuActions[TRAY_ACTION_KICK])
	{
		bool ok;
		QString message = QInputDialog::getText(this, tr("Player name"),
		tr("Player to kick"), QLineEdit::Normal, "", &ok);

		if(!ok)
			return;

		if(ClientGUI* client = getCurrentClient())
			client->commandKick(message);
	}
	else if(action == trayMenuActions[TRAY_ACTION_SETOWNER])
	{
		//TODO
		notImplemented(this);
	}
	else if(action == trayMenuActions[TRAY_ACTION_CLOSE])
	{
		if(ClientGUI* client = getCurrentClient())
		{
			QString ret = "has been closed.";
			if(!client->commandCloseServer())
				ret = "couldnt be closed, an error occured";

			QMessageBox::information(this, "closeserver - OTAdmin", "Server" + ret);
		}
	}
	else if(action == trayMenuActions[TRAY_ACTION_OPEN])
	{
		if(ClientGUI* client = getCurrentClient())
		{
			QString ret = "has been opened.";
			if(client->commandOpenServer())
				ret = "couldnt be opened, an error occured";

			QMessageBox::information(this, "openserver - OTAdmin", "Server " + ret);
		}
	}
	else if(action == trayMenuActions[TRAY_ACTION_PAYHOUSES])
	{
		if(ClientGUI* client = getCurrentClient())
			client->commandPayHouses();
	}
	else if(action == trayMenuActions[TRAY_ACTION_SHUTDOWN])
	{
		if(ClientGUI* client = getCurrentClient())
			client->commandShutdown();
	}

	if(minimized || maximized || restored)
	{
		trayMenuActions[TRAY_ACTION_MINIMIZE]->setDisabled(minimized);
		trayMenuActions[TRAY_ACTION_MAXIMIZE]->setDisabled(maximized);
		trayMenuActions[TRAY_ACTION_RESTORE]->setDisabled(restored);
	}
}

void MainWindow::validateHostEdit(QString text)
{
	hostEdit->setText(text.simplified());
}

void MainWindow::setServerActionsEnabled(bool _enabled)
{
	for(int i = TRAY_ACTION_BROADCAST; i <= TRAY_ACTION_SETOWNER; i++)
		trayMenuActions[i]->setEnabled(_enabled);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
		//case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
		case QSystemTrayIcon::MiddleClick:
			if(isVisible())
			{
				trayMenuActions[TRAY_ACTION_MAXIMIZE]->setDisabled(true);
				showMaximized();
			}
			else
			{
				trayMenuActions[TRAY_ACTION_RESTORE]->setDisabled(true);
				show();
			}

			trayMenuActions[TRAY_ACTION_MINIMIZE]->setDisabled(false);
			break;
		default:
			break;
	}
}

void MainWindow::trayMessageClicked()
{
	ClientGUI *client = findClient(lastHost, lastPort);
	if(!client)
		return;

	if(!isVisible())
		showNormal();

	clientsTab->setCurrentWidget(client);
}

QGroupBox* MainWindow::createConnectGroupBox()
{
	QGroupBox* connectGroupBox = new QGroupBox(tr("Quick connect"), this);

	QLabel* hostLabel = new QLabel("Host:", this);
	QLabel* portLabel = new QLabel("Port:", this);
	QLabel* passwordLabel = new QLabel("Password:", this);

	hostEdit = new QLineEdit("localhost", this);
	portEdit = new QSpinBox(this);
	portEdit->setRange(1, 65536);
	passwordEdit = new QLineEdit("test", this);
	passwordEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);

	hostEdit->setStatusTip("Enter host");
	portEdit->setStatusTip("Enter port");
	passwordEdit->setStatusTip("Enter password");


	QPushButton* connectButton = new QPushButton(tr("Connect"), this);
    connectButton->setStatusTip(("Connect to the server"));

	QHBoxLayout* connectLayout = new QHBoxLayout;
	connectLayout->addWidget(hostLabel);
	connectLayout->addWidget(hostEdit);
	connectLayout->addWidget(portLabel);
	connectLayout->addWidget(portEdit);
	connectLayout->addWidget(passwordLabel);
	connectLayout->addWidget(passwordEdit);
	connectLayout->addWidget(connectButton);

	connectGroupBox->setLayout(connectLayout);

	connect(hostEdit, SIGNAL(textEdited(QString)), this, SLOT(validateHostEdit(QString)));
	connect(connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));

	return connectGroupBox;
}
