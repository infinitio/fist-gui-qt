#include <QApplication>
#include <QClipboard>
#include <QVBoxLayout>

#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/MainView/Links.hh>
#include <fist-gui-qt/MainView/LinkWidget.hh>
#include <fist-gui-qt/notification/LinkReady.hh>
#include <fist-gui-qt/notification/Center.hh>
#include <fist-gui-qt/popup/NoMoreStorage.hh>
#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/MainView/EmptyState/Link.hh>


#include <elle/assert.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.FIST.MainView.Links");

namespace fist
{
  namespace mainview
  {
    Links::Links(fist::State& state,
                 QWidget* parent)
      :  QWidget(parent)
      , _state(state)
      , _link_list(new ListWidget(this, ListWidget::Separator(list::separator::colors), view::background))
      , _widgets()
      , _no_more_storage(new popup::NoMoreStorage(this))
    {
      this->_link_list->setMaxRows(4);
      auto* layout = new QVBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setMargin(0);
      layout->addWidget(this->_link_list);
      this->_no_more_storage->hide();
      if (this->_state.links().get<0>().empty())
      {
        this->_link_list->add_widget(std::make_shared<empty_state::Link>(this));
        return;
      }
      for (model::Link const& model: this->_state.links().get<1>())
        this->add_link(model);
    }

    void
    Links::_copy_link_to_clipboard(uint32_t id)
    {
      ELLE_ASSERT_CONTAINS(this->_state.links().get<0>(), id);
      auto const& link = *this->_state.links().get<0>().find(id);
      fist::notification::center().notify(
        new fist::notification::LinkReady(link, this));
      QClipboard *clipboard = QApplication::clipboard();
      clipboard->setText(link.url().toString());
    }

    void
    Links::add_link(uint32_t id)
    {
      ELLE_ASSERT_CONTAINS(this->_state.links().get<0>(), id);
      this->add_link(*this->_state.links().get<0>().find(id));
    }

    void
    Links::add_link(model::Link const& model)
    {
      // Get rid of the "TextListItem".
      if (this->_widgets.empty())
        this->_link_list->clearWidgets();

      connect(&model, SIGNAL(url_available(uint32_t)),
              this, SLOT(_copy_link_to_clipboard(uint32_t)));
      connect(&model, SIGNAL(payment_required()),
              this, SLOT(_payement_required()));

      if (this->_widgets.find(model.id()) != this->_widgets.end())
      {
        ELLE_WARN("%s: link %s already present", *this, model);
        return;
      }
      auto widget = std::make_shared<LinkWidget>(model);
      connect(widget.get(), SIGNAL(transaction_canceled(uint32_t)),
              &this->_state, SLOT(on_transaction_canceled(uint32_t)));
      connect(widget.get(), SIGNAL(transaction_deleted(uint32_t)),
              &this->_state, SLOT(on_transaction_deleted(uint32_t)));
      this->_link_list->add_widget(widget, ListWidget::Position::Top);
      this->_widgets[model.id()] = widget;
      this->update();
      this->updateGeometry();
    }

    void
    Links::_payement_required()
    {
      QObject* obj = this->sender();
      auto const* link = static_cast<model::Link const*>(obj);
      ELLE_TRACE_SCOPE("payement required: %s", link);
      auto it = this->_widgets.find(link->id());
      if (it != this->_widgets.end())
      {
        this->_link_list->remove_widget(it->second);
        this->_widgets.erase(link->id());
      }
      this->_no_more_storage->show();
    }

    void
    Links::on_link_updated(uint32_t id)
    {
      ELLE_TRACE_SCOPE("%s: update link %s", *this, id);

      if (this->_widgets.find(id) == this->_widgets.end())
      {
        ELLE_WARN("%s: update for an non displayed link: %s",
                  *this, id);
        this->add_link(id);
      }
    }


    void
    Links::print(std::ostream& stream) const
    {
      stream << "Links";
    }
  }
}
