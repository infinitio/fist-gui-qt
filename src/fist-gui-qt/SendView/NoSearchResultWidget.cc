#include <QVBoxLayout>

#include <fist-gui-qt/SendView/NoSearchResultWidget.hh>
#include <fist-gui-qt/SendView/ui.hh>

namespace fist
{
  namespace sendview
  {
    NoSearchResultWidget::NoSearchResultWidget(QWidget* parent)
      : Super(parent, QColor{0xFE, 0xFE, 0xFE}, false)
      , _layout(new QVBoxLayout(this))
    {

      this->setAutoFillBackground(false);
      this->_layout->setSpacing(7);
      this->_layout->setContentsMargins(0, 20, 0, 20);
      auto* title = new QLabel(view::no_result::title::text, this);
      view::no_result::title::style(*title);
      this->_layout->addWidget(title, 0, Qt::AlignCenter);

      auto* subtitle = new QLabel(view::no_result::subtitle::text, this);
      view::no_result::subtitle::style(*subtitle);
      this->_layout->addWidget(subtitle, 0, Qt::AlignCenter);

      auto* icon = new QLabel(this);
      icon->setPixmap(QPixmap(":/send/no-results"));
      this->_layout->addSpacing(5);
      this->_layout->addWidget(icon, 0, Qt::AlignCenter);
    }

    QSize
    NoSearchResultWidget::sizeHint() const
    {
      auto size = this->_layout->minimumSize();
      return QSize(Super::sizeHint().width(), size.height());
    }
  }
}
