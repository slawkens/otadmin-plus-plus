#include <QtWidgets>
#include "aboutwindow.h">
#include "definitions.h"

AboutWindow::AboutWindow(QWidget *parent)
	: QDialog(parent)
{
	QLabel *logoLabel = new QLabel;
	logoLabel->setPixmap(QPixmap(":/images/otadmin.png"));

	QHBoxLayout *aboutWindowLayout = new QHBoxLayout;
	aboutWindowLayout->addWidget(logoLabel);
	aboutWindowLayout->addLayout(createRightPanelLayout());
	aboutWindowLayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(aboutWindowLayout);

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle("About " + qApp->applicationName());
	setWindowFlags(Qt::Dialog
						| Qt::WindowCloseButtonHint);

	exec();
}

QGridLayout *AboutWindow::createRightPanelLayout()
{
    QString os_string = //QSysInfo().prettyProductName();
    #ifdef __WIN32
	QString("Windows");
	#elif __LINUX__
	QString("Linux");
	#else
	QString("Unsupported OS");
	#endif

	QLabel *textLabel = new QLabel;

	textLabel->setText(\
		"This is an <i>OpenTibia Remote-Control</i> tool created by Slawkens.<br>" \
		"Using Qt " + QString(QT_VERSION_STR) + " interface.<br>" \
		"Version <b>" + qApp->applicationVersion() + "</b> for " + os_string + "." \
		"<br><br>"\
		"Published under the GNU General Public License<br>" \
		"This program comes with ABSOLUTELY NO WARRANTY;<br>" \
		"for details see the LICENSE file.<br>" \
		"This is free software, and you are welcome to redistribute it<br>" \
		"under certain conditions." \
		"<br><br>" \
		"<b>Compiled</b> " + __DATE__ + " : " + __TIME__ + " "
		//"<br><br>Visit our site: <a href=http://otadmin.otsoft.org>http://otadmin.otsoft.org</a> for more informations."
	);

    QPushButton *licenseButton = new QPushButton(tr("View License"), this);
    QPushButton *okButton = new QPushButton(tr("OK"), this);

	connect(licenseButton, SIGNAL(clicked()), this, SLOT(licenseClicked()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(close()));

	QGridLayout *rightPanel = new QGridLayout;
	rightPanel->addWidget(textLabel, 0, 0, 1, 2);
	rightPanel->addWidget(licenseButton, 1, 0);
    rightPanel->addWidget(okButton, 1, 1);

	return rightPanel;
}

void AboutWindow::licenseClicked()
{
	QDialog* licenseDialog = new QDialog(this);
	licenseDialog->setMinimumSize(QSize(430, 500));
	licenseDialog->setAttribute(Qt::WA_DeleteOnClose);
	licenseDialog->setWindowTitle("GPLv3 license");
	licenseDialog->setWindowFlags(Qt::Dialog
						| Qt::WindowCloseButtonHint);


	QString licenseText = "The LICENSE.txt file is not available.";
	QFile f(QDir::currentPath() + "/LICENSE.txt");
	if(f.open(QIODevice::ReadOnly))
	{
		licenseText = "";
		QTextStream t(&f);
		while(!t.atEnd())
			licenseText += t.readLine() + "\n";

		f.close();
	}


	QPlainTextEdit* licenseEdit = new QPlainTextEdit(licenseDialog);
	licenseEdit->setReadOnly(true);
	licenseEdit->setPlainText(licenseText);


	QHBoxLayout *licenseLayout = new QHBoxLayout;
	licenseLayout->addWidget(licenseEdit);
	licenseDialog->setLayout(licenseLayout);

	licenseDialog->exec();
}
