#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QtWidgets>

class AboutWindow : public QDialog
{
	Q_OBJECT

	public:
		AboutWindow(QWidget *parent = 0);

	public slots:
		void licenseClicked();

	private:
        QGridLayout *createRightPanelLayout();
};
#endif
