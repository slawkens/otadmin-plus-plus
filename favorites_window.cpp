#include <QtWidgets>
#include "favorites_window.h"

FavoritesWindow::FavoritesWindow(QWidget *parent)
	: QDialog(parent)
{
	QLabel *descLabel = new QLabel("TODO!");

	QHBoxLayout *favoritesWindowLayout = new QHBoxLayout;
	favoritesWindowLayout->addWidget(descLabel);
	favoritesWindowLayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(favoritesWindowLayout);

	setWindowTitle("Favorites - " + qApp->applicationName());
	setWindowFlags(Qt::Dialog
						| Qt::WindowCloseButtonHint);
}
