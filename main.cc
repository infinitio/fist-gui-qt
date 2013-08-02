#include <iostream>

#include <QApplication>
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "AvatarWidget.hh"
#include "DebugWindow.hh"
#include "IconButton.hh"
#include "InfinitDock.hh"
#include "ListWidget.hh"
#include "TransactionPanel.hh"
#include "TransactionWidget.hh"
#include "utils.hh"

int
main(int argc, char** argv)
{
  QApplication app(argc, argv);

  if (QFontDatabase::addApplicationFont(":/fonts/LucidaGrande.ttc") < 0)
    exit(1);

  InfinitDock dock;
  dock.show();

  auto debug = new DebugWindow;
  debug->show();

  auto& transaction_panel = dock.transactionPanel();
  transaction_panel.connect(debug,
                            SIGNAL(addTransaction(QString const&)),
                            SLOT(addTransaction(QString const&)));
  // list->addTransaction("resources/avatar1.png");
  auto t = transaction_panel.addTransaction("resources/avatar2.png");
  // list->addTransaction("resources/avatar3.png");
  t->connect(debug,
             SIGNAL(onProgressChanged(float)),
             SLOT(setProgress(float)));
  return app.exec();
}
