#ifndef PLAYERSWINDOW_H
#define PLAYERSWINDOW_H

#include <QtWidgets>

class QSortFilterProxyModel;

typedef QMap<QString, int> PlayersMap;

class PlayersWindow : public QDialog
{
	Q_OBJECT

	public:
		PlayersWindow(QWidget *_parent = 0, QString _host = "localhost", int _port = 7171);
		bool updateMap();

	private slots:
		void updateClicked();
		void searchEditChanged();

	private:
		QVBoxLayout *createMainLayout();
		QGroupBox *createSearchGroupBox();
		QGroupBox *createPlayersGroupBox();


		QLineEdit *searchEdit;
		PlayersMap playersMap;
		QString host;
		int port;


		// table view
		QStandardItemModel *model;
		void addPlayer(const QString &nick, const int &level);
		void createPlayerModel();
		QSortFilterProxyModel *proxyModel;
};
#endif
