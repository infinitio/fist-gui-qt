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

#include <surface/gap/gap.h>

int
main(int argc, char** argv)
{
  const char* email = "dimrok@infinit.io";
  const char* pw = "bitebite";

  gap_State* state = gap_new();

  char* hash = gap_hash_password(state, email, pw);
  gap_Status status = gap_login(state, email, hash);
  gap_hash_free(hash);

  gap_free(state);

  QApplication app(argc, argv);

  if (QFontDatabase::addApplicationFont(":/fonts/LucidaGrande.ttc") < 0)
    exit(1);

  InfinitDock dock;
  dock.show();

  auto debug = new DebugWindow;
  debug->show();

  auto& transaction_panel = dock.transactionPanel();
  transaction_panel.connect(debug,
                            SIGNAL(addTransaction(Transaction const&)),
                            SLOT(addTransaction(Transaction const&)));
  // list->addTransaction("resources/avatar1.png");
  User* u = new User(QString("MyCure"), QString("resources/avatar3.png"));
  Transaction* trans = new Transaction(QString("firstfile.txt"), *u);
  auto t = transaction_panel.addTransaction(*trans);
  // list->addTransaction("resources/avatar3.png");
  t->connect(debug,
             SIGNAL(onProgressChanged(float)),
             SLOT(setProgress(float)));
  return app.exec();
}
