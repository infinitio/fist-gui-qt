#include <QCoreApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <elle/log.hh>

#include <fist-gui-qt/SearchResultWidget.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.SearchResult");

SearchResultWidget::SearchResultWidget(fist::model::User const& model,
                                       bool preselected,
                                       QWidget* parent):
  ListItem(parent, Qt::white),
  _model(model),
  _avatar(new AvatarIcon(this->_model.avatar(), QSize(32, 32))),
  _selector(new fist::TwoStateIconButton(
    QPixmap(":/buttons/user-unselected@2x.png"),
    QPixmap(":/buttons/user-selected@2x"),
    QPixmap(":/buttons/user-unselected-hover@2x.png"),
    QPixmap(":/buttons/user-selected@2x.png"),
    preselected)),
  _layout(new QHBoxLayout(this))
{
  connect(&this->_model, SIGNAL(avatar_updated()), this, SLOT(_on_avatar_updated()));
  ELLE_TRACE_SCOPE("%s: contruction", *this);

  this->_layout->setSpacing(4);
  this->_layout->setContentsMargins(10, 4, 10, 4);
  this->_layout->addWidget(this->_avatar);
  {
    auto* vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0, 0, 0, 0);
    auto fullname = new QLabel(this->_model.fullname());
    view::send::user::fullname::style(*fullname);
    vlayout->addWidget(fullname);
    this->_layout->addLayout(vlayout, 1);
  }
  this->_layout->addWidget(this->_selector);

#ifndef FIST_PRODUCTION_BUILD
  this->setToolTip(
    QString::fromStdString(
    elle::sprintf("id: %s\nfullname: %s\nhandle: %s",
                    this->_model.id(), this->_model.fullname(), this->_model.handle())));
#endif

connect(this->_selector, SIGNAL(pressed()), this, SLOT(_selected()));
connect(this->_selector, SIGNAL(released()), this, SLOT(_unselected()));
// this->_layout->addWidget(new QLabel(this->_model.fullname()));
  // this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
}

QSize
SearchResultWidget::sizeHint() const
{
  auto size = this->_layout->minimumSize();
  return QSize(Super::sizeHint().width(), size.height());
}

void
SearchResultWidget::enterEvent(QEvent* event)
{
  // Super::enterEvent(event);
  QCoreApplication::sendEvent(this->_selector, event);
}
void
SearchResultWidget::leaveEvent(QEvent* event)
{
  // Super::leaveEvent(event);
  QCoreApplication::sendEvent(this->_selector, event);
}


void
SearchResultWidget::trigger()
{
  ELLE_TRACE_SCOPE("%s: clicked", *this);
  this->_selector->click();
}

void
SearchResultWidget::_selected()
{
  emit selected(this->_model.id());
}

void
SearchResultWidget::_unselected()
{
  emit unselected(this->_model.id());
}

void
SearchResultWidget::_on_avatar_updated()
{
  this->_avatar->set_avatar(this->_model.avatar());
  this->update();
}

void
SearchResultWidget::_update()
{
  ELLE_TRACE_SCOPE("%s: update", *this);
}

void
SearchResultWidget::print(std::ostream& stream) const
{
  stream << "SearchResult(" << this->_model << ")";
}
