//
// Created by igor on 2/11/24.
//

#include "navigation_panel.hh"
#include "navigation_model.hh"
#include "navigation_view.hh"

NavigationPanel::NavigationPanel (QWidget* parent)
	: QFrame (parent),
	  m_is_active(true) {
	m_layout = new QVBoxLayout(this);
	m_model = new NavigationModel("/");
	m_view = new NavigationView();
	m_view->setModel (m_model);
	m_layout->addWidget (m_view);

	setFrameStyle (QFrame::Box | QFrame::Raised);

	connect (m_model, &NavigationModel::modelPopulated, m_view, &NavigationView::onModelPopulated);
	connect (m_model, &NavigationModel::cursorMoved, m_view, &NavigationView::onCursorMoved);
	connect (m_view, &NavigationView::navigateDown, m_model, &NavigationModel::onNavigateDown);
	connect (m_view, &NavigationView::navigateUp, m_model, &NavigationModel::onNavigateUp);
	connect (m_view, &NavigationView::drillDown, m_model, &NavigationModel::onDrillDown);
	connect (m_view, &NavigationView::drillDownByIndex, m_model, &NavigationModel::onDrillDownByIndex);
	connect (m_model, &NavigationModel::currentPathChanged, this, [this](const QString& newPath) {
		emit currentPathChanged(newPath);
	});
}

void NavigationPanel::setActive(bool flag) {
	m_is_active = flag;
	m_model->setActive (m_is_active);
	if (flag) {
		this->setFocus ();
		m_view->setFocus ();
	}
}

void NavigationPanel::toggleActive () {
	setActive (!m_is_active);
}

QString NavigationPanel::getCurrentPath () const {
	return m_model->getCurrentPath();
}