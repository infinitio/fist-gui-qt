#include <QCoreApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <elle/log.hh>

#include <fist-gui-qt/SendView/SearchResultWidget.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/SendView/ui.hh>

ELLE_LOG_COMPONENT("infinit.FIST.SearchResult");

namespace fist
{
  namespace sendview
  {
    _SearchResultWidget::_SearchResultWidget(bool preselected,
                                             QWidget* parent)
      : ListItem(parent, Qt::white)
      , _fullname(new QLabel(this))
      , _avatar(new AvatarIcon(QPixmap(), QSize(25, 25), this))
      , _selector(new fist::TwoStateIconButton(
                    ":/send/user-selected",
                    ":/send/user-selected-active",
                    preselected,
                    this,
                    QSize(23, 23)))
      , _layout(new QHBoxLayout(this))
    {
      ELLE_TRACE_SCOPE("%s: contruction", *this);

      this->_layout->setSpacing(8);
      this->_layout->setContentsMargins(10, 4, 10, 4);
      this->_layout->addWidget(this->_avatar, 0, Qt::AlignCenter);
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->setSpacing(0);
        vlayout->setContentsMargins(0, 0, 0, 0);
        view::user::fullname::style(*this->_fullname);
        vlayout->addWidget(this->_fullname);
        this->_layout->addLayout(vlayout, 1);
      }
      this->_layout->addWidget(this->_selector);

      connect(this->_selector, SIGNAL(pressed()), this, SLOT(_selected()));
      connect(this->_selector, SIGNAL(released()), this, SLOT(_unselected()));
    }

    QSize
    _SearchResultWidget::sizeHint() const
    {
      auto size = this->_layout->minimumSize();
      return QSize(Super::sizeHint().width(), size.height());
    }

    void
    _SearchResultWidget::enterEvent(QEvent* event)
    {
      // Super::enterEvent(event);
      QCoreApplication::sendEvent(this->_selector, event);
    }
    void
    _SearchResultWidget::leaveEvent(QEvent* event)
    {
      // Super::leaveEvent(event);
      QCoreApplication::sendEvent(this->_selector, event);
    }

    void
    _SearchResultWidget::trigger()
    {
      ELLE_TRACE_SCOPE("%s: clicked", *this);
      this->_selector->click();
    }

    void
    _SearchResultWidget::_update()
    {
      ELLE_TRACE_SCOPE("%s: update", *this);
    }

    void
    _SearchResultWidget::print(std::ostream& stream) const
    {
      stream << "SearchResult";
    }

    SearchResultWidget::SearchResultWidget(fist::model::User const& model,
                                           bool preselected,
                                           QWidget* parent)
      : Super(preselected, parent)
      , _model(model)
    {
      this->_fullname->setText(this->_model.fullname());
      this->_avatar->set_avatar(model.avatar());
      connect(&this->_model, SIGNAL(avatar_updated()),
              this, SLOT(_on_avatar_updated()));

#ifndef FIST_PRODUCTION_BUILD
      this->setToolTip(
        QString::fromStdString(elle::sprintf("%s", this->_model)));
#endif

      if (this->_model.me())
      {
        this->_fullname->setText("Me (my other devices)");
      }
    }

    void
    SearchResultWidget::_on_avatar_updated()
    {
      this->_avatar->set_avatar(this->_model.avatar());
      this->update();
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
    SearchResultWidget::print(std::ostream& stream) const
    {
      stream << "SearchResult(" << this->_model << ")";
    }

  }
}
