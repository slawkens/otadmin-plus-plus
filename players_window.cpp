#include <QtWidgets>

#include "players_window.h"
#include "client.h"
#include "networkmessage.h"
#include "definitions.h"

PlayersWindow::PlayersWindow(QWidget *_parent, QString _host, int _port)
	: QDialog(_parent)
{
	host = _host, port = _port;
	setLayout(createMainLayout());

	setWindowTitle(host + ":" + QString::number(port) + " - players list - OTAdmin");
	setWindowFlags(Qt::Window);
	resize(QSize(300, 500));

	updateMap();
}

bool PlayersWindow::updateMap()
{
	QByteArray result;
	connectionError_t ret = Client::getServerInfo(host, port, REQUEST_EXT_PLAYERS_INFO, result);

	if(ret != CONNECT_SUCCESS)
		return false;

	playersMap.clear();

	NetworkMessage msg(result);
	uint32_t playersCount = msg.GetU32();
	for(uint32_t i = 0; i < playersCount; i++)
	{
		QString name = QString::fromStdString(msg.GetString());
		uint32_t level = msg.GetU32();
		playersMap.insertMulti(name, level);
	}

	if(model->rowCount() > 0)
		model->removeRows(0, model->rowCount());

	PlayersMap::const_iterator it = playersMap.constBegin();
	while(it != playersMap.constEnd())
	{
		addPlayer(it.key(), it.value());
		++it;
	}

//	model->index(1, 0)->setDisabled(true);
	return true;
}

void PlayersWindow::updateClicked()
{
	if(!updateMap())
		return;
}

QVBoxLayout *PlayersWindow::createMainLayout()
{
	QPushButton *updateButton = new QPushButton("Update");
	updateButton->setAutoDefault(false);
	connect(updateButton, SIGNAL(clicked()), this, SLOT(updateClicked()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(createSearchGroupBox());
	mainLayout->addWidget(createPlayersGroupBox());
	mainLayout->addWidget(updateButton);
	return mainLayout;
}

QGroupBox *PlayersWindow::createSearchGroupBox()
{
	QGroupBox *searchGroupBox = new QGroupBox("Search player");

	searchEdit = new QLineEdit;

	QVBoxLayout *searchLayout = new QVBoxLayout;
	searchLayout->addWidget(searchEdit);

	searchGroupBox->setLayout(searchLayout);
	searchGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	return searchGroupBox;
}

QGroupBox *PlayersWindow::createPlayersGroupBox()
{
	QGroupBox *playersGroupBox = new QGroupBox("Players list");

	proxyModel = new QSortFilterProxyModel;
	proxyModel->setDynamicSortFilter(true);

	QTreeView *playersView = new QTreeView;
	playersView->setRootIsDecorated(false);
	playersView->setAlternatingRowColors(true);
	playersView->setModel(proxyModel);
	playersView->setSortingEnabled(true);
	playersView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	playersView->setColumnWidth(0, 100);
	playersView->setColumnWidth(1, 100);

	connect(searchEdit, SIGNAL(textChanged(const QString &)),
		this, SLOT(searchEditChanged()));

	QVBoxLayout *playersLayout = new QVBoxLayout;
	playersLayout->addWidget(playersView);
	playersGroupBox->setLayout(playersLayout);

	createPlayerModel();
	proxyModel->setSourceModel(model);
	proxyModel->setFilterKeyColumn(0);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

	return playersGroupBox;
}

void PlayersWindow::searchEditChanged()
{
	QRegExp regExp(searchEdit->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
	proxyModel->setFilterRegExp(regExp);
}

void PlayersWindow::addPlayer(const QString &nick, const int &level)
{
	model->insertRow(0);
	model->setData(model->index(0, 0), nick);
	model->setData(model->index(0, 1), level);
}

void PlayersWindow::createPlayerModel()
{
	model = new QStandardItemModel(0, 2, this);

	model->setHeaderData(0, Qt::Horizontal, QObject::tr("Player name"));
	model->setHeaderData(1, Qt::Horizontal, QObject::tr("Level"));
}
