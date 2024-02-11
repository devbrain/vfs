//
// Created by igor on 2/7/24.
//
#include <algorithm>

#include <QKeyEvent>
#include <QFont>
#include <QItemSelection>
#include <QStyledItemDelegate>
#include "navigation_view.hh"

class CursorPainter : public QStyledItemDelegate {
 public:
	explicit CursorPainter (QWidget* parent);
	void paint (QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

CursorPainter::CursorPainter (QWidget* parent)
	: QStyledItemDelegate (parent) {
}

void CursorPainter::paint (QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {

	QStyleOptionViewItem newOption = option;

	if (newOption.state & QStyle::State_HasFocus) {
		newOption.state = newOption.state ^ QStyle::State_HasFocus;
	}

	if (qvariant_cast<bool> (index.data (CURSOR_ROLE))) {
		newOption.font.setWeight (QFont::Bold);
		newOption.font.setUnderline (true);
	}
	QStyledItemDelegate::paint (painter, newOption, index);

}

// =======================================================================================
NavigationView::NavigationView (QWidget* parent)
	: QTreeView (parent),
	  m_current_pos (0) {
	setSelectionBehavior (SelectRows);
	setSelectionMode (NoSelection);
	setAutoScroll (true);

	setItemDelegate (new CursorPainter (this));

	m_keys_map = {
		{Qt::Key_Up, [this] () { emit navigateUp (Step); }},
		{Qt::Key_Down, [this] () { emit navigateDown (Step); }},
		{Qt::Key_PageUp, [this] () { emit navigateUp (Jump); }},
		{Qt::Key_PageDown, [this] () { emit navigateDown (Jump); }},
		{Qt::Key_Home, [this] () { emit navigateUp (Edge); }},
		{Qt::Key_End, [this] () { emit navigateDown (Edge); }},
		{Qt::Key_Insert, [this] () { doSelect (); }},
		{Qt::Key_Asterisk, [this] () { invertSelection (); }},
		{Qt::Key_Enter, [this] () { emit drillDown (); }},
		{Qt::Key_Return, [this] () { emit drillDown (); }},
	};
}

void NavigationView::keyPressEvent (QKeyEvent* e) {

	const auto key = e->key ();
	auto itr = m_keys_map.find (key);
	if (itr != m_keys_map.end ()) {
		itr->second ();
	} else {
		QTreeView::keyPressEvent (e);
	}

}

void NavigationView::mouseDoubleClickEvent (QMouseEvent* e) {
	if (e->button () == Qt::LeftButton) {
		emit drillDownByIndex (indexAt(e->pos()));
	}
	QTreeView::mouseDoubleClickEvent (e);
}

void NavigationView::onModelPopulated () {
	//highlightRow (0, true);
}

void NavigationView::doSelect () {
	const auto lastColumn = model ()->columnCount (QModelIndex ()) - 1;
	auto startIndex = model ()->index (m_current_pos, 0);
	auto endIndex = model ()->index (m_current_pos, lastColumn);
	QItemSelection selection (startIndex, endIndex);
	selectionModel ()->select (selection, QItemSelectionModel::Toggle);
	emit navigateDown (Step);
}

void NavigationView::invertSelection () {
	const auto count = model ()->rowCount (QModelIndex ());
	if (count > 0) {
		const auto lastColumn = model ()->columnCount (QModelIndex ()) - 1;
		auto startIndex = model ()->index (0, 0);
		auto endIndex = model ()->index (count - 1, lastColumn);
		QItemSelection selection (startIndex, endIndex);
		selectionModel ()->select (selection, QItemSelectionModel::Toggle);
	}
}

void NavigationView::onCursorMoved (const QModelIndex& newPos) {
	scrollTo (newPos);
	m_current_pos = newPos.row ();
}