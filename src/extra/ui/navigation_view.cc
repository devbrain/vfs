//
// Created by igor on 2/7/24.
//

#include <QKeyEvent>
#include "navigation_view.hh"

NavigationView::NavigationView (QWidget* parent)
	: QTreeView(parent) {
	setAutoScroll (true);
}

void NavigationView::keyPressEvent (QKeyEvent* e) {
	bool handeled = false;
	if(e->key()==Qt::Key_Up) {
		handeled = true;
		emit onKeyUp();
	} else if (e->key()==Qt::Key_Down) {
		handeled = true;
		emit onKeyDown();
	}
	if (!handeled) {
		QTreeView::keyPressEvent (e);
	}
}
