#include <fist-gui-qt/AddFileWidget.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>

#include <elle/log.hh>

#include <QDropEvent>
#include <QHBoxLayout>
#include <QPalette>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QUrl>

ELLE_LOG_COMPONENT("infinit.FIST.AddFileWidget");

AddFileWidget::AddFileWidget(QWidget* parent):
  QWidget(parent),
  _text(new QLabel(view::send::file_adder::text)),
  _attach(new IconButton(QPixmap(":/icons/files.png")))
{
  // Background.
  {
    QPalette palette = this->palette();
    {
      palette.setColor(QPalette::Window, view::send::file_adder::background);
    }
    this->setPalette(palette);
    this->setAutoFillBackground(true);
  }

  // Text.
  {
    view::send::file_adder::style(*this->_text);
  }

  auto* layout = new QHBoxLayout(this);
  layout->setAlignment(Qt::AlignVCenter);
  this->setContentsMargins(6, 0, 6, 0);
  layout->addWidget(this->_attach);
  layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::Minimum, QSizePolicy::Maximum));
  layout->addWidget(this->_text);
  this->setFixedWidth(320);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  this->setAcceptDrops(true);
};

IconButton*
AddFileWidget::attach()
{
  return this->_attach;
}

QSize
AddFileWidget::sizeHint() const
{
  return QSize(this->width(), 42);
}

void
AddFileWidget::pulse()
{
  QPropertyAnimation* animation = new QPropertyAnimation(this, "pulseColor");
  animation->setDuration(600);
  animation->setEasingCurve(QEasingCurve::InOutQuad);
  animation->setStartValue(QColor(0xBC, 0xD2, 0xD6, 0x77));
  animation->setEndValue(view::send::file_adder::background);
  animation->start();
}

void
AddFileWidget::setPulseColor(QColor const& color)
{
  QPalette palette;
  palette.setColor(QPalette::Background, color);
  this->setAutoFillBackground(true);
  this->setPalette(palette);
}

void
AddFileWidget::enterEvent(QEvent*)
{
  this->on_entered();
  this->setCursor(QCursor(Qt::PointingHandCursor));
}

void
AddFileWidget::leaveEvent(QEvent*)
{
  this->on_left();
  this->setCursor(QCursor(Qt::ArrowCursor));
}

void
AddFileWidget::mousePressEvent(QMouseEvent*)
{
  emit clicked();
}

void
AddFileWidget::on_entered()
{
  ELLE_DEBUG_SCOPE("%s: mouse entered", *this);
  view::send::file_adder::hover_style(*this->_text);
  this->_attach->set_pixmap(QPixmap(":/icons/files-hover.png"));
  this->repaint();
}

void
AddFileWidget::on_left()
{
  ELLE_DEBUG_SCOPE("%s: mouse left", *this);
  view::send::file_adder::style(*this->_text);
  this->_attach->set_pixmap(QPixmap(":/icons/files.png"));
  this->repaint();
}

void
AddFileWidget::dropEvent(QDropEvent *event)
{
  ELLE_TRACE_SCOPE("%s: drop", *this);

  if (event->mimeData()->hasUrls())
    for (auto const& url: event->mimeData()->urls())
      if (url.isLocalFile())
      {
        event->acceptProposedAction();
        ELLE_DEBUG("%s dropped", url);
        emit file_dropped(url);
      }
  view::send::file_adder::style(*this->_text);
}

void
AddFileWidget::print(std::ostream& stream) const
{
  stream << "File adder";
}
