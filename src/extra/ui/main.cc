//
// Created by igor on 2/4/24.
//

// #include "mainwindow.hh"
#include <QApplication>
#include <QKeyEvent>

#include <vfs/system.hh>
#include "mainwindow.hh"

class VFSCommander : public QApplication {
 public:
	VFSCommander (int& argc, char** argv, int flags = ApplicationFlags);

	int run() const;
 private:
	bool notify (QObject* receiver, QEvent* event) override;
	static void setupVFS();
 private:
	MainWindow* m_main_window;
};

VFSCommander::VFSCommander (int& argc, char** argv, int flags)
	: QApplication (argc, argv, flags),
	  m_main_window(nullptr) {
	setupVFS();
	m_main_window = new MainWindow(nullptr);
}

bool VFSCommander::notify (QObject* receiver, QEvent* event) {
	if (event && event->type () == QEvent::KeyPress) {
		auto* keyEvent = dynamic_cast<QKeyEvent*>(event);
		if (keyEvent && keyEvent->key () == Qt::Key_Tab) {
			m_main_window->onTabPressed();
			return true;
		}
	}
	return QApplication::notify (receiver, event);
}

int VFSCommander::run () const{
	m_main_window->show();
	return exec ();
}

void VFSCommander::setupVFS () {
	vfs::mount ("physfs", "/home/igor", "/");
}

int main (int argc, char* argv[]) {
	VFSCommander application (argc, argv);
	return application.run();
}
