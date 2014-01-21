#include <fist-gui-qt/AddFileWidget.hh>
#include <fist-gui-qt/globals.hh>

#include <QPropertyAnimation>
#include <QPalette>
#include <QPixmap>
#include <QHBoxLayout>

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
  this->setContentsMargins(6, 0, 6, 0);
  layout->addWidget(this->_attach);
  layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::Minimum, QSizePolicy::Maximum));
  layout->addWidget(this->_text);
  this->setFixedWidth(320);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
  QPropertyAnimation* animation =
    new QPropertyAnimation(this, "pulseColor");
  animation->setDuration(600);
  animation->setEasingCurve(QEasingCurve::InOutQuad);
  animation->setStartValue(QColor(0xBC, 0xD2, 0xD6, 0x77));
  animation->setEndValue(Qt::GlobalColor::white);
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
