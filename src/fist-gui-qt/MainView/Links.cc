#include <QApplication>
#include <QClipboard>
#include <QVBoxLayout>

#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/MainView/Links.hh>
#include <fist-gui-qt/MainView/LinkWidget.hh>
#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/globals.hh>

#include <elle/assert.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.FIST.Transactions");

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
    {
      this->_link_list->setMaxRows(4);
      auto* layout = new QVBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setMargin(0);
      layout->addWidget(this->_link_list);

      if (this->_state.links().get<0>().empty())
      {
        this->_link_list->add_widget(
          std::make_shared<TextListItem>("You have no links yet", 70, this));
        return;
      }

      for (model::Link const& model: this->_state.links().get<0>())
      {
        if (model.status() != gap_transaction_canceled &&
            model.status() != gap_transaction_failed &&
            model.status() != gap_transaction_deleted)
          this->add_link(model);
      }
    }

    void
    Links::add_link(uint32_t id)
    {
      ELLE_ASSERT_CONTAINS(this->_state.links().get<0>(), id);
      this->add_link(*this->_state.links().get<0>().find(id));

      emit systray_message(
        SystrayMessageCarrier(
          new Message("Link ready!",
                      "Link has been copied to your clipboard")));
      QClipboard *clipboard = QApplication::clipboard();
      clipboard->setText(this->_state.links().get<0>().find(id)->url().toString());
    }

    void
    Links::add_link(model::Link const& model)
    {
      if (this->_widgets.empty())
      {
        // Get rid of the "TextListItem".
        this->_link_list->clearWidgets();
      }

      if (this->_widgets.find(model.id()) != this->_widgets.end())
      {
        ELLE_WARN("%s: link %s already present", *this, model);
      }

      auto widget = std::make_shared<LinkWidget>(model);

      connect(widget.get(), SIGNAL(transaction_canceled(uint32_t)),
              &this->_state, SLOT(on_transaction_canceled(uint32_t)));
      connect(widget.get(), SIGNAL(transaction_deleted(uint32_t)),
              &this->_state, SLOT(on_transaction_deleted(uint32_t)));

      this->_link_list->add_widget(widget,
                                   ListWidget::Position::Top);
      this->_widgets[model.id()] = widget;
      this->update();
      this->updateGeometry();
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
