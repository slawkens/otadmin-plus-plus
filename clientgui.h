#ifndef CLIENTGUI_H
#define CLIENTGUI_H

#include <QWidget>
#include <QtWidgets>

#include "client.h"
#include "clientstab.h"
#include "mainwindow.h"

enum
{
	SERVERINFO_STATUS = 0,
	SERVERINFO_UPTIME,
	//SERVERINFO_IP,
	SERVERINFO_SERVERNAME,
	//SERVERINFO_PORT,
	SERVERINFO_LOCATION,
	SERVERINFO_URL,
	SERVERINFO_SERVER,
//	SERVERINFO_VERSION,
//	SERVERINFO_CLIENT,

	SERVERINFO_OWNER,
//	SERVERINFO_OWNER_NAME,
//	SERVERINFO_OWNER_EMAIL,

	SERVERINFO_PLAYERS,
//	SERVERINFO_PLAYERS_ONLINE,
//	SERVERINFO_PLAYERS_MAX,
//	SERVERINFO_PLAYERS_PEAK,

	SERVERINFO_MONSTERS_TOTAL,

	SERVERINFO_NPCS_TOTAL,

	SERVERINFO_MAP_NAME,
	SERVERINFO_MAP_AUTHOR,
	SERVERINFO_MAP_WIDTH,
	SERVERINFO_MAP_HEIGHT,

	SERVERINFO_MOTD,
	SERVERINFO_LAST = SERVERINFO_MOTD
};

class Client;
class ClientsTab;

class ClientGUI : public QWidget, public virtual Client
{
	Q_OBJECT

	public:
		ClientGUI(ClientsTab* _parent = 0, QString _host = "localhost", int _port = 7171, QString _password = "test");

		QHBoxLayout* createMainLayout();

		QTextEdit* getConsole() {return console;}
		void consoleLog(QString message);

		void requestStatusInfo();
		void updateStatusInfo();
		void internalUpdateStatusInfo(StatusInfo_t statusInfo);

		QString transformUptime(int uptime);

		bool disconnect() {/*parent->onClientClose(); */return Client::disconnect();}

	public slots:
		void statusInfoTimer();
		void pingTimer();

		void playersClicked();

	protected:
		ClientsTab* parent;

	private:
		QWidget* playersWindow;

		QGroupBox *createStatusGroupBox();
		QGroupBox *createLuaGroupBox();
		QGroupBox *createSqlGroupBox();

		bool displayTrayWarning;

		QPushButton *playersButton;

		QTextEdit *console, *lua, *sql;

		QLabel *serverInfo[SERVERINFO_LAST + 1];
		QLabel* statusErrorLabel;
};
#endif
