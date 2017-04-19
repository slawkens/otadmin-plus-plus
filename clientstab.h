#ifndef CLIENTSTAB_H
#define CLIENTSTAB_H

#include <QMainWindow>
#include <QTabWidget>

#include "mainwindow.h"
#include "clientgui.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QToolButton;
class QSpinBox;
QT_END_NAMESPACE

class MainWindow;
class ClientGUI;
class ClientsTab : public QTabWidget
{
	Q_OBJECT

	public:
		ClientsTab(QWidget *parent);

		void wannaClose(ClientGUI* client);
		void onClientClose();

		ClientGUI* getCurrentClient();

		QIcon m_connectedIcon[2];
		QMenu* tabCustomMenu;
		MainWindow* parent;

	public slots:
		void disconnectAll();
		void currentChanged(int index);
		void tabCloseRequested(int index);

		void displayTabMenu(QPoint);

	protected:
		void mousePressEvent(QMouseEvent * event);
};
#endif
