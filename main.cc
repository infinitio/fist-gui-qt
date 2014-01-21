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

#include <elle/log.hh>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/DebugWindow.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/InfinitDock.hh>
#include <fist-gui-qt/LoginWindow.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/utils.hh>

#include <surface/gap/gap.h>

ELLE_LOG_COMPONENT("infinit.FIST");

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
  ELLE_LOG("start FIST gui");
  // Connect to gap.
  gap_State* state = gap_new();

  // Open qt application.
  QApplication app(argc, argv);

  QFont arial("Arial", 11);
  QApplication::setFont(arial);

  app.setWindowIcon(QIcon(QPixmap(":/images/logo.png")));
  app.setQuitOnLastWindowClosed(false);
  // Open login window, which will trigger the InfinitDock later on.
  auto login = new LoginWindow(state);
  center_window(login);
  login->show();

  char ret = app.exec();
  gap_free(state);
  return ret;
}
