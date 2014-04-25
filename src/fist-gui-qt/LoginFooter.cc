#include <fist-gui-qt/LoginFooter.hh>
#include <fist-gui-qt/globals.hh>

#include <QCursor>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

#include <iostream>

LoginFooter::LoginFooter(QWidget* parent):
  Footer(parent)
{
  auto text = new QLabel(view::login::footer::text);
  view::login::footer::style(*this);
  this->_layout->addWidget(text, 0, Qt::AlignCenter);
}

void
LoginFooter::click()
{
  if (this->isEnabled())
    emit released();
}

void
LoginFooter::mouseReleaseEvent(QMouseEvent*)
{
  this->click();
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
