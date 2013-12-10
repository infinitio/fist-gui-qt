#include <fist-gui-qt/LoginFooter.hh>
#include <fist-gui-qt/globals.hh>

#include <QCursor>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

#include <iostream>

LoginFooter::LoginFooter():
  Footer()
{
  auto text = new QLabel(QString("Login"));
  view::login::footer::style(*this);
  this->_layout->addWidget(text, 0, Qt::AlignCenter);
}

void
LoginFooter::mouseReleaseEvent(QMouseEvent*)
{
  emit released();
}

void
LoginFooter::enterEvent(QEvent*)
{
  this->setCursor(QCursor(Qt::PointingHandCursor));
}

void
LoginFooter::leaveEvent(QEvent*)
{
  this->setCursor(QCursor(Qt::ArrowCursor));
}
