#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <iostream>

#include <fist-gui-qt/SearchResultWidget.hh>
#include <fist-gui-qt/globals.hh>

SearchResultWidget::SearchResultWidget(UserModel const& model,
                       QWidget* parent):
  ListItem(parent, Qt::white),
  _model(model),
  _avatar(new AvatarIcon(this->_model.avatar(), QSize(32, 32))),
  _layout(new QHBoxLayout(this))
{

  this->_layout->setContentsMargins(10, 10, 10, 10);
  this->_layout->addWidget(this->_avatar);
  this->_layout->addSpacing(10);

  {
    auto texts = new QVBoxLayout();
    {
      auto fullname = new QLabel(this->_model.fullname());
      view::send::user::fullname::style(*fullname);
      texts->addWidget(fullname);
    }
    {
      auto handle = new QLabel(this->_model.fullname());
      view::send::user::handle::style(*handle);
      texts->addWidget(handle);
    }
    this->_layout->addLayout(texts);
  }
  // this->_layout->addWidget(new QLabel(this->_model.fullname()));
  // this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
}

QSize
SearchResultWidget::sizeHint() const
{
  auto size = this->_layout->minimumSize();
  return QSize(this->widthHint(), size.height());
}

void
SearchResultWidget::trigger()
{
  std::cout << "clicked: " << this->_model.fullname().toStdString() << std::endl;
  emit clicked_signal(this->_model.id());
}

void
SearchResultWidget::update()
{
  std::cerr << "SearchResultWidget: " << this << " update" << std::endl;
  if (this->_model.new_avatar())
  {
    std::cerr << "new avatar available" << std::endl;
    this->_avatar->set_avatar(this->_model.avatar());
  }
}
