#include <QtWidgets>
#include "splashscreen.h"

SplashScreen::SplashScreen()
	: QSplashScreen()
{
	/*
		Up panel
	*/
	QHBoxLayout *upPanel = new QHBoxLayout;

	QLabel *logo = new QLabel;
	logo->setAlignment(Qt::AlignCenter);
	logo->setPixmap(QPixmap(":/images/otadmin.png"));

	QLabel *descLabel = new QLabel;
	descLabel->setAlignment(Qt::AlignCenter);
	descLabel->setText("Version <b>" + qApp->applicationVersion() + "</b>");

	upPanel->addWidget(logo);
	upPanel->addWidget(descLabel);

	/*
		Down panel
	*/
	QHBoxLayout *downPanel = new QHBoxLayout;

	QLabel *infoLabel = new QLabel;
	infoLabel->setText(qApp->applicationName() + " comes with ABSOLUTELY NO WARRANTY. This is free software and you are welcome to redisturbe it under certain circumstances.<br>see the GNU General Public License in About menu for details.");

	downPanel->addWidget(infoLabel);

	/*
		Main layout
	*/
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(upPanel);
	mainLayout->addLayout(downPanel);
	mainLayout->setAlignment(Qt::AlignCenter);
	setLayout(mainLayout);

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);

	QDesktopWidget *screen = QApplication::desktop();
	show();
	move(QPoint(screen->width() / 2, screen->height() / 2) -
			QPoint(width() / 2,height() / 2 ) );

	qApp->processEvents();
}
