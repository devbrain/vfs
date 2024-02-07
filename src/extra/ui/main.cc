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
	QObject::connect (&w, &NavigationView::onKeyUp, ds, &NavigationModel::moveUp);
	QObject::connect (&w, &NavigationView::onKeyDown, ds, &NavigationModel::moveDown);
	w.setModel (ds);
	w.show ();

	return QApplication::exec ();
}
