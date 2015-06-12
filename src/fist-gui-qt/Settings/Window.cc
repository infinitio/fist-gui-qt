#include <iostream>

#include <QBuffer>
#include <QDialog>
#include <QTabWidget>
#include <QFileDialog>
#include <QCoreApplication>
#include <QSpacerItem>
#include <QEvent>
#include <QKeyEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPainter>
#include <QStackedLayout>

#include <elle/container/map.hh>

#include <fist-gui-qt/AvatarIcon.hh>
#include <fist-gui-qt/Settings/Window.hh>
#include <fist-gui-qt/Settings/General.hh>
#include <fist-gui-qt/Settings/Profile.hh>
#include <fist-gui-qt/Settings/ScreenShot.hh>
#include <fist-gui-qt/Settings/utils.hh>
#include <fist-gui-qt/State.hh>

namespace fist
{
  namespace prefs
  {

    TabBar::TabBar(QWidget* parent)
      : QTabBar(parent)
    {
      new QStackedLayout(this);
      this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      this->setFixedHeight(100);
    }

    void
    TabBar::paintEvent(QPaintEvent* event)
    {
      QTabBar::paintEvent(event);
    }

    void
    TabWidget::paintEvent(QPaintEvent* event)
    {
      QPainter painter(this);
      painter.setPen(QColor(242, 94, 90));
      painter.setBrush(QColor(242, 94, 90));
      painter.drawRect(0, 0, this->width(), this->height());
      QTabWidget::paintEvent(event);
    }

    Window::Window(fist::State& state,
                   QWidget* parent)
      : QDialog(parent)
      , _state(state)
      , _tabWidget(new TabWidget(this))
    {
      auto* layout = new QHBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->addWidget(this->_tabWidget);
      this->_tabWidget->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding);
      this->_tabWidget->addTab(new General(this->_state, this), tr("General"));
      this->_tabWidget->addTab(new Profile(this->_state, this), tr("Profile"));
      this->_tabWidget->addTab(new ScreenShot(this->_state, this), tr("Screenshots"));
      this->_tabWidget->setStyleSheet(
        "QTabBar {"
        "  background-color: rgb(242, 94, 90);"
        "  width: 999999px;"
        "}"
        "QTabBar::tab {"
        "  border: 0px;"
        "  height: 100px;"
        "  width: 100px;"
        "  background-color: rgb(242, 94, 90);"
        "  padding: 0px;"
        "  margin: 0px;"
        "  color: white;"
        "  font: bold 13px;"
        "}"
        "QTabBar::tab:selected, QTabBar::tab:hover {"
        " background-color: rgb(213, 86, 83);"
        "}"
        "QTabBar::tab:focus {"
        "  border: 0px;"
        "}"
        );
    }

  }
}
