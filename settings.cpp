//#include <QtWidgets>
#include <QSettings>

#include "mainwindow.h"

void MainWindow::readSettings()
{
	QSettings settings(QApplication::applicationDirPath() + "/otadmin.ini", QSettings::IniFormat);
	settings.beginGroup("main");

    //QMessageBox::information(this, "PATH TEST", QDir::currentPath());
    //QMessageBox::information(this, "PATH TEST222", QApplication::applicationDirPath());
	QString style = settings.value("style", "Cleanlooks").toString();

	QList<QAction*> actions = stylesMenu->actions();
	foreach(QAction* action, actions)
	{
		if(action->text() == style)
			action->setChecked(true);
	}

	currentStyle = style;
	changeStyle(style);

	QString styleSheet = settings.value("stylesheet", "").toString();
	currentStyleSheet = styleSheet;
//	changeStyleSheet(styleSheet);

	move(settings.value("pos", QPoint(100, 80)).toPoint());
	resize(settings.value("size", QSize(280, 160)).toSize());
	displayTrayNotify = settings.value("trayNotify", true).toBool();

	settings.endGroup();

	settings.beginGroup("lastserver");

	hostEdit->setText(settings.value("host", "localhost").toString());
	portEdit->setValue(settings.value("port", "7171").toInt());
    //passwordEdit->setText(settings.value("password", "test").toString());

	settings.endGroup();
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.beginGroup("main");

	settings.setValue("style", currentStyle);
//	settings.setValue("stylesheet", currentStyleSheet);

	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("trayNotify", displayTrayNotify);

	settings.endGroup();

	settings.beginGroup("lastserver");

	settings.setValue("host", hostEdit->text());
	settings.setValue("port", portEdit->value());
    //settings.setValue("password", passwordEdit->text());

	settings.endGroup();
}
