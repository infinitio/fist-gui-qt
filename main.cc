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
#include "LoginWindow.hh"
#include "ListWidget.hh"
#include "TransactionPanel.hh"
#include "TransactionWidget.hh"
#include "utils.hh"

#include <surface/gap/gap.h>

int
main(int argc, char** argv)
{
  // Connect to gap.
  gap_State* state = gap_new();

  // Open qt application.
  QApplication app(argc, argv);
  if (QFontDatabase::addApplicationFont(":/fonts/LucidaGrande.ttc") < 0)
    exit(1);

  // Open login window, which will trigger the InfinitDock later on.
  auto login = new LoginWindow(state);
  login->show();

  return app.exec();
}
