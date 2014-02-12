#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <elle/log.hh>

#include <fist-gui-qt/SearchResultWidget.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.SearchResult");

SearchResultWidget::SearchResultWidget(UserModel const& model,
                                       QWidget* parent):
  ListItem(parent, Qt::white),
  _model(model),
  _avatar(new AvatarIcon(this->_model.avatar(), QSize(32, 32))),
  _layout(new QHBoxLayout(this))
{
  ELLE_TRACE_SCOPE("%s: contruction", *this);

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
      auto handle = new QLabel(this->_model.handle());
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
  ELLE_TRACE_SCOPE("%s: clicked", *this);
  emit clicked_signal(this->_model.id());
}

void
SearchResultWidget::_update()
{
  ELLE_TRACE_SCOPE("%s: update", *this);
  if (this->_model.new_avatar())
  {
    ELLE_DEBUG("avatar available");
    this->_avatar->set_avatar(this->_model.avatar());
  }
}

void
SearchResultWidget::print(std::ostream& stream) const
{
  stream << "SearchResult(" << this->_model << ")";
}
