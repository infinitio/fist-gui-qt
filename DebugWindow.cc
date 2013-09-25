#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>

#include "DebugWindow.hh"

DebugWindow::DebugWindow()
{
  auto add = new QPushButton("Add transaction");

  auto central = new QWidget;
  this->setCentralWidget(central);
  auto layout = new QVBoxLayout(central);
  layout->addWidget(add);

  connect(add, SIGNAL(clicked()), SLOT(_add_transaction()));

  auto progress = new QScrollBar(Qt::Horizontal);
  this->connect(progress,
                SIGNAL(valueChanged(int)),
                SLOT(_change_progress(int)));
  layout->addWidget(progress);
}

void
DebugWindow::_add_transaction()
{
  User* u = new User(QString("mefyl"), "resources/avatar2.png");
  Transaction* t = new Transaction(QString("kikoo.jpg"), *u);
  Q_EMIT addTransaction(*t);
}

void
DebugWindow::_change_progress(int value)
{
  Q_EMIT onProgressChanged(value / 100.);
}
