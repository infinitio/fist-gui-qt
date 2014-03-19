#include <QApplication>
#include <QMainWindow>

#include <elle/log.hh>

#include <surface/gap/gap.h>

#include <exp/FakeUserModel.hh>
#include <exp/FakeTransactionModel.hh>

#include <fist-gui-qt/UserWidget.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/Panel.hh>
#include <fist-gui-qt/RoundShadowWidget.hh>

ELLE_LOG_COMPONENT("infinit.exp.FIST");

int
main(int argc, char** argv)
{
  ELLE_LOG("foooooooooo");

  QApplication application(argc, argv);

  FakeTransactionModel porn{1, true, {"porn.avi"}};
  FakeTransactionModel movie{2, false, {"movie.avi"}, "foo bar", gap_transaction_pending};
  FakeTransactionModel pics{3, false, {"pics.png", "pacs.png"}};
  FakeTransactionModel porn2{4, true, {"porn.avi"}, "This is child porn", gap_transaction_waiting_for_accept};
  FakeTransactionModel movie2{5, false, {"movie.avi"}, "foo bar", gap_transaction_waiting_for_accept};
  FakeTransactionModel pics2{6, false, {"pics.png", "pacs.png"}, "pics of sonia", gap_transaction_finished};
  FakeTransactionModel porn3{7, true, {"porn.avi"}, "This is porn, and this is a really long fucking message note from the peer.", gap_transaction_failed};
  FakeTransactionModel movie3{8, false, {"movie.avi"}, "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM", gap_transaction_rejected};
  FakeTransactionModel pics3{9, false, {"pics.png", "pacs.png"}};

  FakeUserModel bertier{1, "bertier", "la cogip", {porn, pics, movie2, pics2, porn3, movie3}};
  FakeUserModel sonja{2, "sonja", "Petit Paon", {movie, porn2, pics3}};

  QMainWindow w0;

  auto* users = new ListWidget(&w0);
  users->add_widget(new UserWidget(bertier));
  users->add_widget(new UserWidget(sonja));
  w0.setCentralWidget(users);
  w0.show();

  RoundShadowWidget w1;
  // auto* footer = new Footer;
  // auto* panel = new Panel(footer);
  // w1.setCentralWidget(panel);
  auto* transactions = new ListWidget(&w1, ListWidget::Separator());
  transactions->add_widget(new TransactionWidget{porn});
  transactions->add_widget(new TransactionWidget{movie});
  transactions->add_widget(new TransactionWidget{pics});
  transactions->add_widget(new TransactionWidget{porn2});
  transactions->add_widget(new TransactionWidget{movie2});
  transactions->add_widget(new TransactionWidget{pics2});
  transactions->add_widget(new TransactionWidget{porn3});
  transactions->add_widget(new TransactionWidget{movie3});
  transactions->add_widget(new TransactionWidget{pics3});

  w1.setCentralWidget(transactions);
  w1.show();

  return application.exec();
}
