#include <QHBoxLayout>
#include <QLabel>

#include "AvatarWidget.hh"
#include "TransactionWidget.hh"

TransactionWidget::TransactionWidget(Transaction const& t):
  _transaction(t),
  _avatar(new AvatarWidget),
  _layout(nullptr)
{
  static int const padding = 5;

  this->_avatar->setPicture(t.user.avatar);

  auto layout = new QHBoxLayout(this);
  this->_layout = layout;
  layout->addWidget(this->_avatar);

  {
    auto texts = new QVBoxLayout;
    texts->setContentsMargins(5, 12, 5, 12);
    layout->addLayout(texts);
    {
      auto username = new QLabel(t.user.name);
      QFont font;
      font.setBold(true);
      username->setFont(font);
      username->move(this->_avatar->width() + padding, padding);
      texts->addWidget(username);
    }
    {
      auto filename = new QLabel(t.filename);
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
  std::cout << "trigger transaction" << std::endl;
  auto win = new TransactionWindow(_transaction, this);
  win->show();
}
