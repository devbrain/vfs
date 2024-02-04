//
// Created by igor on 2/4/24.
//

#include "mainwindow.hh"
#include <QApplication>

int main (int argc, char* argv[]) {
	QApplication a (argc, argv);
	MainWindow w;
	w.show ();

	return QApplication::exec ();
}
