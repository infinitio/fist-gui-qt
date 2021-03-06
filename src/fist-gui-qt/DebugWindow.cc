#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>

#include <fist-gui-qt/DebugWindow.hh>

DebugWindow::DebugWindow()
{
  auto add = new QPushButton("Add transaction", this);

  auto central = new QWidget(this);
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
  Q_EMIT addTransaction(nullptr, 0);
}

void
DebugWindow::_change_progress(int value)
{
  Q_EMIT onProgressChanged(value / 100.);
}
