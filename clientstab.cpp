#include <QtWidgets>

#include "clientstab.h"
#include "clientgui.h"


ClientsTab::ClientsTab(QWidget *_parent)
	: QTabWidget(_parent)
{
	parent = (MainWindow*)_parent;

	setTabsClosable(true);
	setMovable(true);

	m_connectedIcon[0] = QIcon(":/images/not_connected.png");
	m_connectedIcon[1] = QIcon(":/images/connected.png");

	QPushButton *disconnectAllButton = new QPushButton("Disconnect all", this);
	disconnectAllButton->setToolTip("Disconnecting all clients");

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(disconnectAllButton);
	buttonLayout->setMargin(0);
	buttonLayout->setSpacing(0);

	QWidget *_cornerWidget = new QWidget;
	_cornerWidget->setLayout(buttonLayout);
	setCornerWidget(_cornerWidget, Qt::TopRightCorner);

//	tabCustomMenu = new QMenu(this);
//	QList<QAction*> tabActions;
//	tabActions.append(new QAction("Disconnect", this));
//	tabActions.append(new QAction("Reconnect", this));
//	tabCustomMenu->addActions(tabActions);
//	setContextMenuPolicy(Qt::CustomContextMenu);
//	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayTabMenu(QPoint)));

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
	connect(disconnectAllButton, SIGNAL(clicked()), this, SLOT(disconnectAll()));

	//setIconSize(QSize(65, 64));
	//setTabPosition(QTabWidget::West);
	//setTabShape(QTabWidget::Triangular);
}

ClientGUI* ClientsTab::getCurrentClient()
{
	if(ClientGUI* tmp = (ClientGUI*)currentWidget())
		return tmp;

	return NULL;
}

void ClientsTab::mousePressEvent(QMouseEvent* event)
{
	if(event->button() == Qt::MidButton)
	{
		int index = tabBar()->tabAt(event->pos());
		if(index == -1)
			return;

		if(ClientGUI* client = (ClientGUI*)widget(index))
			wannaClose(client);
	}
}

void ClientsTab::tabCloseRequested(int index)
{
	ClientGUI* client = (ClientGUI*)this->widget(index);
	if(!client)
		return;

	wannaClose(client);
}

void ClientsTab::wannaClose(ClientGUI* client)
{
	bool close = true;
	if(client->isConnected())
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("Confirm");
		msgBox.setText("Are you sure you want to close connection with this server?");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();
		if(ret == QMessageBox::Cancel)
			close = false;
	}

	if(close)
	{
		client->disconnect();
		onClientClose();
		delete client;
	}
}

void ClientsTab::onClientClose()
{
	bool disable = false;
	if(tabBar()->count() <= 1)
	{
		parent->setWindowTitle(qApp->applicationName());
		disable = true;
	}

	if(!disable)
	{
		if(ClientGUI* client = getCurrentClient())
		{
			if(client->isConnected())
				disable = false;
		}
	}

	if(disable)
		parent->setServerActionsEnabled(false);
}

void ClientsTab::currentChanged(int index)
{
	if(ClientGUI* client = getCurrentClient())
	{
		parent->setWindowTitle(client->getHost() + ":" + QString::number(client->getPort()) + " -" + qApp->applicationName());
		parent->setServerActionsEnabled(client->isConnected());
	}
}

void ClientsTab::disconnectAll()
{
	ClientGUI* client = (ClientGUI*) currentWidget();
	while(client != NULL)
	{
		client->disconnect();
		onClientClose();
		delete client;

		client = (ClientGUI*) currentWidget();
	}
}

void ClientsTab::displayTabMenu(QPoint point)
{
	if (this->tabBar()->tabAt(point) != -1)// && e->button() == Qt::RightButton)
		tabCustomMenu->exec(QCursor::pos());
}
