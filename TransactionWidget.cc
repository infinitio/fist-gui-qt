#include <QHBoxLayout>
#include <QLabel>

#include "AvatarWidget.hh"
#include "TransactionWidget.hh"

TransactionWidget::TransactionWidget(uint32_t tid):
  _tid(tid),
  _avatar(new AvatarWidget),
  _layout(nullptr)
{
  static int const padding = 5;

  // TODO: retrieve avatar from gap
  auto tmp_avatar = QPixmap(QString("resources/avatar2.png"));
  this->_avatar->setPicture(tmp_avatar);

  auto layout = new QHBoxLayout(this);
  this->_layout = layout;
  layout->addWidget(this->_avatar);

  {
    auto texts = new QVBoxLayout;
    texts->setContentsMargins(5, 12, 5, 12);
    layout->addLayout(texts);
    {
      auto username = new QLabel(QString("tmp_username_hehe"));
      QFont font;
      font.setBold(true);
      username->setFont(font);
      username->move(this->_avatar->width() + padding, padding);
      texts->addWidget(username);
    }
    {
      auto filename = new QLabel(QString("tmp_filename_hehe"));
      QFont font;
      filename->setFont(font);
      QPalette palette;
      palette.setColor(QPalette::WindowText, QColor(150, 150, 150));
      filename->setPalette(palette);
      filename->move(this->_avatar->width() + padding,
                     this->_avatar->height() - filename->height() - padding);
      texts->addWidget(filename);
    }
  }

  setSizePolicy(QSizePolicy::MinimumExpanding,
                QSizePolicy::Fixed);
  adjustSize();

  connect(this->_avatar,
          SIGNAL(onProgressChanged(float)),
          SIGNAL(onProgressChanged(float)));
}

/*-----------.
| Properties |
`-----------*/

float
TransactionWidget::progress() const
{
  return this->_avatar->progress();
}

void
TransactionWidget::setProgress(float value)
{
  this->_avatar->setProgress(value);
}

/*-------.
| Layout |
`-------*/

QSize
TransactionWidget::sizeHint() const
{
  auto size = this->_layout->minimumSize();
  return QSize(300, size.height());
}

QSize
TransactionWidget::minimumSizeHint() const
{
  return this->sizeHint();
}

void
TransactionWidget::trigger()
{
  auto pop = new QWidget(parentWidget());
  parentWidget()->hide();
  pop->show();
}
