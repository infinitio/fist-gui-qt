#include <QDialogButtonBox>
#include <QMovie>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QPixmap>
#include <QMessageBox>
#include <QPainter>

#include <elle/log.hh>

#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/LoadingDialog.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.LoadingWindow");

LoadingDialog::LoadingDialog(QObject* parent):
  QDialog(nullptr, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
  _text(new QLabel(tr("Search for update"), this)),
  _body(new QTextEdit(this)),
  _loading_icon(new QLabel(this)),
  _accept_button(new QPushButton(tr("Accept"), this)),
  _reject_button(new QPushButton(tr("Reject"), this))
  , _progress_bar(new QProgressBar(this))
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
    this->_loading_icon->setMovie(new QMovie(QString(":/icons/loading")));
    this->_loading_icon->movie()->start();
    layout->addWidget(this->_loading_icon, 0, Qt::AlignCenter);
  }

  {
    this->_body->setReadOnly(true);
    this->_body->setAcceptRichText(true);
    layout->addWidget(this->_body, 0, Qt::AlignCenter);
    this->_text->setFrameStyle(QFrame::NoFrame);
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
  layout->addStretch();
  {
    this->_progress_bar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
    layout->addWidget(this->_progress_bar);
    this->_progress_bar->hide();
  }
  this->layout()->setSizeConstraint(QLayout::SetFixedSize);

  this->loading_mode();
}

void
LoadingDialog::download_progress(qint64 bytesReceived, qint64 bytesTotal)
{
  ELLE_DEBUG_SCOPE("%s: progress: %s / %s", *this, bytesReceived, bytesTotal);
  this->_progress_bar->setMaximum(bytesTotal);
  this->_progress_bar->setValue(bytesReceived);
  this->_progress_bar->show();
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
  this->_progress_bar->hide();
  this->adjustSize();
}

void
LoadingDialog::loading_mode()
{
  ELLE_TRACE_SCOPE("%s: loading mode", *this);

  this->_loading_icon->show();
  this->_accept_button->hide();
  this->_reject_button->hide();
  if (!this->_body->toHtml().isEmpty())
    this->_body->show();
  this->_progress_bar->show();
  this->adjustSize();
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
    QDialog::close();
}

void
LoadingDialog::closeEvent(QCloseEvent * event)
{
  ELLE_TRACE_SCOPE("%s: close event aborted", *this);

  QMessageBox b(QMessageBox::Question,
                tr("Quit"),
                tr("Are you sure you want to quit?"),
                QMessageBox::Ok | QMessageBox::Cancel,
                this);
  connect(&b, SIGNAL(accepted()),
          this, SIGNAL(quit_request()));
  auto res = b.exec();
  if (res == QMessageBox::Ok)
    emit quit_request();
  else
    event->ignore();
}

void
LoadingDialog::resizeEvent(QResizeEvent* event)
{
  ELLE_TRACE("resized");
  if (event->spontaneous())
  {
    Super::resizeEvent(event);
  }
  emit resized();
}

void
LoadingDialog::close()
{
  ELLE_TRACE_SCOPE("%s: hide loading dialog", *this);
  this->done(0);
}
