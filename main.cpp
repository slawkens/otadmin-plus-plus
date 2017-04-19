#include <QtWidgets>
//#include "qtsingleapplication/src/qtsingleapplication.h"
#include "mainwindow.h"
#include "splashscreen.h"

int main(int argc, char *argv[])
{
    //QtSingleApplication app(argc, argv);
    QApplication app(argc, argv);

	QString message;
	for (int a = 1; a < argc; ++a)
	{
		message += argv[a];
		if (a < argc-1)
			message += " ";
	}

    //if(app.sendMessage(message))
    //	return 0;

	app.setApplicationName("OTAdmin++");
	app.setApplicationVersion("0.0.1 ALPHA");
//	app.setOrganizationDomain("http://otadmin.otsoft.org");
	app.setOrganizationName("OTSoft.org");
    app.setWindowIcon(QIcon(":/images/otadmin.ico"));

	//SplashScreen *splash = new SplashScreen;
	//QSplashScreen splash(QPixmap(":/images/splash.png"), Qt::WindowStaysOnTopHint);
	//splash.show();
	//app.processEvents();

	if(!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Systray"),
							  QObject::tr("I couldn't detect any system tray "
										  "on this system."));
		return 1;
	}
	app.setQuitOnLastWindowClosed(false);

	MainWindow mainWindow;
	mainWindow.show();
//	QTimer::singleShot(5000, &splash, SLOT(close()));

//	app.setActivationWindow(&mainWindow);
//	QObject::connect(&app, SIGNAL(messageReceived(const QString&)),
        //	&mainWindow, SLOT(handleMessage(const QString&)));
//	QObject::connect(&mainWindow, SIGNAL(needToShow()), &app, SLOT(showNormal()));

	return app.exec();
}
