#ifndef LOADINGDIALOG_HH
# define LOADINGDIALOG_HH

# include <QDialog>
# include <QLabel>
# include <QPushButton>
# include <QTextEdit>

# include <elle/attribute.hh>

# include <fist-gui-qt/fwd.hh>

class LoadingDialog:
  public QDialog
{
  typedef QDialog Super;

public:
  LoadingDialog(QObject* parent);

public Q_SLOTS:
  void
  accept_reject_mode(bool mandatory);

  void
  loading_mode();

  void
  text(QString const& text);

  void
  body(QString const& text);

signals:
  void
  quit_request();

public Q_SLOTS:
  void
  close();

private:
  void
  keyPressEvent(QKeyEvent*) override;

  void
  closeEvent(QCloseEvent*) override;

  void
  resizeEvent(QResizeEvent*) override;

signals:
  void
  resized();

private:
  ELLE_ATTRIBUTE(QLabel*, text);
  ELLE_ATTRIBUTE(QTextEdit*, body);
  ELLE_ATTRIBUTE(QLabel*, loading_icon);
  ELLE_ATTRIBUTE_R(QPushButton*, accept_button);
  ELLE_ATTRIBUTE_R(QPushButton*, reject_button);

private:
  Q_OBJECT
};


#endif
