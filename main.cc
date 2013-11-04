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
#include <QDesktopWidget>
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

static
void
center_window(QWidget* widget)
{
  int width = widget->frameGeometry().width();
  int height = widget->frameGeometry().height();

  QDesktopWidget wid;

  int screenWidth = wid.screen()->width();
  int screenHeight = wid.screen()->height();

  widget->setGeometry((screenWidth / 2) - (width / 2),
                      (screenHeight / 2) - (height / 2),
                      width,
                      height);
}

int
main(int argc, char** argv)
{
  // Connect to gap.
  gap_State* state = gap_new();

  // Open qt application.
  QApplication app(argc, argv);
  if (QFontDatabase::addApplicationFont(":/fonts/LucidaGrande.ttc") < 0)
    exit(1);

  app.setWindowIcon(QIcon(QPixmap(":/images/logo.png")));
  // Open login window, which will trigger the InfinitDock later on.
  auto login = new LoginWindow(state);
  center_window(login);
  login->show();

  char ret = app.exec();
  gap_free(state);
  return ret;
}
