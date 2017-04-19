#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include <QTabWidget>
#include <QDialog>

#include "clientgui.h"
#include "mainwindow.h"

#include <stdio.h>
#include <sys/types.h>

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QToolButton;
class QSpinBox;
QT_END_NAMESPACE

enum trayActions_t
{
	TRAY_ACTION_FIRST = 0,
	TRAY_ACTION_MINIMIZE = TRAY_ACTION_FIRST,
	TRAY_ACTION_MAXIMIZE,
	TRAY_ACTION_RESTORE,
	TRAY_ACTION_QUIT,
	TRAY_ACTION_BROADCAST,
	TRAY_ACTION_CLOSE,
	TRAY_ACTION_OPEN,
	TRAY_ACTION_PAYHOUSES,
	TRAY_ACTION_SHUTDOWN,
	TRAY_ACTION_KICK,
	TRAY_ACTION_SETOWNER,
	TRAY_ACTION_LAST = TRAY_ACTION_SETOWNER
};

enum openedWindows_t
{
	OPENED_WINDOW_FAVORITES = 0,
	OPENED_WINDOW_LAST = 0
};

class Client;
class ClientGUI;
class ClientsTab;
class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();

		void notImplemented(QWidget* window);

		void setServerActionsEnabled(bool _enabled);
		QSystemTrayIcon* getTrayIcon() {return trayIcon;}
		//QAction* getTrayMenuAction(trayActions_t v) {return trayMenuActions[v];}
		ClientGUI *getCurrentClient();
		ClientGUI *findClient(QString host, int port);

		QString lastHost;
		int lastPort;

	public slots:
		void connectClicked();

		void trayMessageClicked();
		void iconActivated(QSystemTrayIcon::ActivationReason reason);
		void trayIconMenuTriggered(QAction* action);
		void validateHostEdit(QString text);

		void exitApp();
		void commandExecutor();
		//void favoritesClicked();
		void launchWebsite();
		void about();
		void notImplemented();
		void changeStyle(QAction *act);

		//siglestone app
		void handleMessage(const QString& message);

	signals:
		void needToShow();
		//end siglestone app

	protected:
		void closeEvent(QCloseEvent* event);

		void readSettings();
		void writeSettings();

		void changeStyle(QString style);

	private:
		void createMenus();
		void createTrayMenus();

		ClientsTab* clientsTab;

		QWidget *openedWindows[OPENED_WINDOW_LAST + 1];
		QGroupBox* createConnectGroupBox();

		QLineEdit *hostEdit, *passwordEdit;
		QSpinBox* portEdit;

		QString currentStyle, currentStyleSheet;

		///////////////////////////////
		// Menus
		///////////////////////////////
		QMenu *mainMenu, *serverMenu, *stylesMenu, *helpMenu;

		QAction *commandsAct,
			*favoritesAct,
			*exitAct,
			*connectAct,
			*styleActs[3],
			*websiteAct, *aboutAct;
		///////////////////////////////

		///////////////////////////////
		// Tray menu
		///////////////////////////////
		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu;

		QAction* trayMenuActions[TRAY_ACTION_LAST + 1];
		QList<QAction*> serverCommandsAct;
		bool displayTrayNotify;
		///////////////////////////////
};
#endif
