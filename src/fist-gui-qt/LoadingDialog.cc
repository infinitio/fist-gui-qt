#include <QDialogButtonBox>
#include <QMovie>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QPixmap>
#include <QPainter>

#include <elle/log.hh>

#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/LoadingDialog.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.LoadingWindow");

LoadingDialog::LoadingDialog(QObject* parent):
  QDialog(nullptr, Qt::CustomizeWindowHint | Qt::WindowTitleHint),
  _text(new QLabel(tr("Search for update"), this)),
  _body(new QTextEdit(this)),
  _loading_icon(new QLabel(this)),
  _accept_button(new QPushButton(tr("Accept"), this)),
  _reject_button(new QPushButton(tr("Reject"), this))
{
  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);

  {
    QPalette palette = this->palette();
    {
      palette.setColor(QPalette::Window, view::background);
      palette.setColor(QPalette::Base, view::background);
    }
    this->setPalette(palette);
  }

  {
    layout->addWidget(this->_text, 0, Qt::AlignCenter);
    QPalette palette = this->_text->palette();
    {
      palette.setColor(QPalette::Window, view::background);
    }
    this->_text->setPalette(palette);
  }

  layout->addSpacing(8);

  {
    this->_loading_icon->setMovie(new QMovie(QString(":/icons/loading.gif")));
    this->_loading_icon->movie()->start();
    layout->addWidget(this->_loading_icon, 0, Qt::AlignCenter);
  }

  {
    this->_body->setReadOnly(true);
    this->_body->setAcceptRichText(true);
    layout->addWidget(this->_body, 0, Qt::AlignCenter);
    this->_text->setFrameStyle(QFrame::NoFrame);
    this->_body->setFixedWidth(180);
    this->_body->setSizePolicy(QSizePolicy::Fixed,
                               QSizePolicy::MinimumExpanding);
  }
  layout->addStretch();
  {
    auto* hlayout = new QHBoxLayout;

    hlayout->addStretch();
    hlayout->addWidget(this->_accept_button, 0, Qt::AlignCenter);
    hlayout->addWidget(this->_reject_button, 0, Qt::AlignCenter);
    hlayout->addStretch();

    layout->addLayout(hlayout);
  }

  this->loading_mode();
}

void
LoadingDialog::accept_reject_mode(bool mandatory)
{
  ELLE_TRACE_SCOPE("%s: accept/reject mode", *this);

  this->_loading_icon->hide();
  if (!this->_body->toHtml().isEmpty())
    this->_body->show();
  this->_accept_button->show();
  if (!mandatory)
    this->_reject_button->show();

  // this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
  this->adjustSize();
  // this->show();
}

void
LoadingDialog::loading_mode()
{
  ELLE_TRACE_SCOPE("%s: loading mode", *this);

  this->_loading_icon->show();
  this->_accept_button->hide();
  this->_reject_button->hide();
  this->_body->hide();
  // this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
  this->adjustSize();
  // this->show();
}

void
LoadingDialog::text(QString const& text)
{
  ELLE_TRACE_SCOPE("%s: update title with text %s", *this, text);

  QPalette palette = this->_text->palette();
  {
    palette.setColor(QPalette::Text, Qt::red);
  }
  this->_text->setPalette(palette);

  this->_text->setText(text);
  this->updateGeometry();
}

void
LoadingDialog::body(QString const& text)
{
  ELLE_TRACE_SCOPE("%s: update body with text %s", *this, text);

  this->_body->setHtml(text);
  this->_body->adjustSize();
  this->updateGeometry();
}

void
LoadingDialog::keyPressEvent(QKeyEvent* event)
{
  ELLE_TRACE_SCOPE("%s: key pressed (%s)", *this, event->key());

  if (event->key() == Qt::Key_Escape && !this->_reject_button->isHidden())
    this->_reject_button->click();
  else if (event->key() == Qt::Key_Return && !this->_accept_button->isHidden())
    this->_accept_button->click();
  else if (event->key() == Qt::Key_Escape &&
           (this->windowFlags() & Qt::WindowCloseButtonHint))
    this->close();
}

void
LoadingDialog::closeEvent(QCloseEvent * event)
{
  ELLE_TRACE_SCOPE("%s: close event aborted", *this);
  event->ignore();
}

void
LoadingDialog::resizeEvent(QResizeEvent* event)
{
  Super::resizeEvent(event);
  emit resized();
}

void
LoadingDialog::close()
{
  ELLE_TRACE_SCOPE("%s: hide loading dialog", *this);
  this->hide();
}
