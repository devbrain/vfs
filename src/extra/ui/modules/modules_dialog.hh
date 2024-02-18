//
// Created by igor on 2/16/24.
//

#ifndef VFS_SRC_EXTRA_UI_MODULES_MODULES_DIALOG_HH_
#define VFS_SRC_EXTRA_UI_MODULES_MODULES_DIALOG_HH_

#include <QDialog>

namespace Ui {
	class ModulesDialog;
}

class ModulesModel;

class ModulesDialog : public QDialog {
 public:
	explicit ModulesDialog (QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
 private:
	void loadModuleFromFile();
	void loadModuleFromDirectory();
 private:
	Ui::ModulesDialog* ui;
	ModulesModel* m_model;
};

#endif //VFS_SRC_EXTRA_UI_MODULES_MODULES_DIALOG_HH_
