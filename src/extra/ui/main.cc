//
// Created by igor on 2/4/24.
//

// #include "mainwindow.hh"
#include <QApplication>

#include <vfs/system.hh>
#include "navigation_model.hh"
#include "navigation_view.hh"

int main (int argc, char* argv[]) {
	QApplication a (argc, argv);

	vfs::mount("physfs", "/home/igor", "/");
	auto* ds = new NavigationModel("/");

	NavigationView w;
	QObject::connect (ds, &NavigationModel::modelPopulated, &w, &NavigationView::onModelPopulated);
	QObject::connect (ds, &NavigationModel::cursorMoved, &w, &NavigationView::onCursorMoved);
	QObject::connect (&w, &NavigationView::navigateDown, ds, &NavigationModel::onNavigateDown);
	QObject::connect (&w, &NavigationView::navigateUp, ds, &NavigationModel::onNavigateUp);
	QObject::connect (&w, &NavigationView::drillDown, ds, &NavigationModel::onDrillDown);
	QObject::connect (&w, &NavigationView::drillDownByIndex, ds, &NavigationModel::onDrillDownByIndex);

	w.setModel (ds);
	w.onModelPopulated();
	w.show ();

	return QApplication::exec ();
}
