//
// Created by igor on 2/11/24.
//

#ifndef VFS_SRC_EXTRA_UI_NAVIGATION_CURSOR_HH_
#define VFS_SRC_EXTRA_UI_NAVIGATION_CURSOR_HH_

#include <QtCore>

constexpr inline auto CURSOR_ROLE = Qt::UserRole + 1;
enum JumpMode {
	Step,
	Jump,
	Edge
};

#endif //VFS_SRC_EXTRA_UI_NAVIGATION_CURSOR_HH_
