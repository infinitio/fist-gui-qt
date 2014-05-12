#include <iostream>

#include <elle/log.hh>

#include <surface/gap/gap.hh>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/SendFooter.hh>
#include <fist-gui-qt/SendPanel.hh>
#include <fist-gui-qt/gui/Tooltip.hh>
#include <fist-gui-qt/TransactionFooter.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/SearchField.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/AddFileWidget.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/onboarding/Onboarder.hh>

ELLE_LOG_COMPONENT("infinit.FIST.onboarding.Onboarder");

namespace fist
{
  namespace onboarding
  {
    Onboarder::Onboarder(InfinitDock* dock)
      : QObject()
      , _dock(dock)
      , _transactions()
      , _tooltip(nullptr)
    {
      ELLE_TRACE_SCOPE("%s: creation", *this);
      connect(&this->_dock->transactionPanel(), SIGNAL(new_transaction(uint32_t)),
              this, SLOT(_on_new_transaction(uint32_t)));
      connect(&this->_dock->transactionPanel(), SIGNAL(new_transaction(uint32_t)),
              this, SLOT(_on_new_transaction(uint32_t)));
      connect(&this->_dock->transactionPanel(), SIGNAL(new_transaction_shown(TransactionWidget*)),
              this, SLOT(_on_transaction_widget_shown(TransactionWidget*)));
    }

    Onboarder::~Onboarder()
    {}

    void
    Onboarder::receive_file(QString const& file)
    {
      this->_transactions[gap_onboarding_receive_transaction(
          this->_dock->_state.state(), file.toStdString().c_str(), 6)] = nullptr;
    }

    void
    Onboarder::_on_new_transaction(uint32_t id)
    {
      if (this->_transactions.find(id) == this->_transactions.end())
        return;
    }

    void
    Onboarder::_on_transaction_widget_shown(TransactionWidget* widget)
    {
      auto const& model = widget->transaction();
      if (this->_transactions.find(model.id()) == this->_transactions.end())
        return;
      this->_transactions[model.id()] = widget;
      connect(widget, SIGNAL(transaction_accepted(uint32_t)),
              this, SLOT(_on_transaction_accepted(uint32_t)));
      connect(widget, SIGNAL(transaction_rejected(uint32_t)),
              this, SLOT(_on_transaction_rejected(uint32_t)));
      connect(widget, SIGNAL(transaction_canceled(uint32_t)),
              this, SLOT(_on_transaction_canceled(uint32_t)));
      connect(widget, SIGNAL(transaction_finished(uint32_t)),
              this, SLOT(_on_transaction_finished(uint32_t)));

      this->_set_tooltip(
        widget->accept_button(), "Click here to accept files.", Qt::AlignLeft);
      connect(widget->accept_button(), SIGNAL(clicked()),
              this->_tooltip.get(), SLOT(hide()));
    }

    void
    Onboarder::_on_transaction_accepted(uint32_t id)
    {
      if (this->_transactions.find(id) == this->_transactions.end())
        return;
      this->_set_tooltip(
        this->_transactions[id]->peer_avatar(),
        "The file is being saved to\n your Downloads folder.",
        Qt::AlignLeft, 6000);
      connect(this, SIGNAL(reception_completed()),
              this->_tooltip.get(), SLOT(hide()));
    }

    void
    Onboarder::_on_transaction_rejected(uint32_t id)
    {
      if (this->_transactions.find(id) == this->_transactions.end())
        return;
      this->_set_tooltip(
        this->_transactions[id]->status(), "Wow that was harsh.",
        Qt::AlignLeft, 4000);
      emit reception_completed();
    }

    void
    Onboarder::_on_transaction_canceled(uint32_t id)
    {
      if (this->_transactions.find(id) == this->_transactions.end())
        return;
      this->_set_tooltip(
        this->_transactions[id]->status(), "Wow that was harsh.",
        Qt::AlignLeft, 4000);
      emit reception_completed();
    }

    void
    Onboarder::_on_transaction_finished(uint32_t id)
    {
      if (this->_transactions.find(id) == this->_transactions.end())
        return;
      emit reception_completed();
    }

    void
    Onboarder::send_file()
    {
      auto* send_button = this->_dock->transactionPanel().footer()->send();
      this->_set_tooltip(
        send_button, "To send a file, click here", Qt::AlignLeft);
      connect(send_button, SIGNAL(clicked()),
              this->_tooltip.get(), SLOT(hide()));
      connect(this->_dock->_send_panel, SIGNAL(shown()),
              this, SLOT(_on_send_panel_visible()));
    }

    void
    Onboarder::_on_send_panel_visible()
    {
      auto* widget = this->_dock->_send_panel;

      disconnect(this->_dock->_send_panel, SIGNAL(shown()),
                 this, SLOT(_on_send_panel_visible()));

      this->_choose_peer();
      connect(widget, SIGNAL(peer_found()),
              this, SLOT(_on_peer_chosen()));
      connect(widget, SIGNAL(file_added()),
              this, SLOT(_on_file_added_before_peer()));
      connect(widget, SIGNAL(sent()),
              this, SLOT(_send_onboarding_done()));
      connect(widget, SIGNAL(canceled()),
              this, SLOT(_send_onboarding_done()));
    }

    void
    Onboarder::_choose_peer()
    {
      this->_set_tooltip(
        this->_dock->_send_panel->search(),
        "Search for a friend using\n"
        "his fullname or nickname.",
        Qt::AlignLeft);
    }

    void
    Onboarder::_on_peer_chosen()
    {
      auto* send_panel = this->_dock->_send_panel;
      if (send_panel->files().isEmpty())
      {
        this->_set_tooltip(
          send_panel->file_adder(),
          "Add files by clicking the icon.\n"
          "You can also drop them on that window.",
          Qt::AlignLeft);
        connect(send_panel->file_adder(), SIGNAL(clicked()),
                this->_tooltip.get(), SLOT(hide()));
      }
      else
      {

        this->_on_file_added_before_peer();
      }

      disconnect(send_panel, SIGNAL(peer_found()),
                 this, SLOT(_on_peer_chosen()));
      disconnect(send_panel, SIGNAL(file_added()),
                 this, SLOT(_on_file_added_before_peer()));
      connect(send_panel, SIGNAL(file_added()),
              this, SLOT(_on_transaction_ready()));
    }

    void
    Onboarder::_on_file_added()
    {
      auto* send_panel = this->_dock->_send_panel;
      this->_on_transaction_ready();
      connect(send_panel->footer()->send(), SIGNAL(clicked()),
              this->_tooltip.get(), SLOT(hide()));
    }

    void
    Onboarder::_on_file_added_before_peer()
    {
      auto* send_panel = this->_dock->_send_panel;
      this->_set_tooltip(
        send_panel->file_adder(),
        "Oh, you already get how to add files!",
        Qt::AlignLeft,
        1500);

      if (send_panel->peer_valid())
      {
        connect(this->_tooltip.get(), SIGNAL(hidden()),
                this, SLOT(_on_transaction_ready()));
      }
      else
      {
        connect(this->_tooltip.get(), SIGNAL(hidden()),
                this, SLOT(_choose_peer()));
      }

      disconnect(send_panel, SIGNAL(file_added()),
                 this, SLOT(_on_file_added_before_peer()));
      connect(send_panel, SIGNAL(peer_found()),
              this, SLOT(_on_transaction_ready()));
    }

    void
    Onboarder::_on_transaction_ready()
    {
      auto* send_panel = this->_dock->_send_panel;
      this->_set_tooltip(
        send_panel->footer()->send(),
        "If everything is ready, click here.",
        Qt::AlignLeft);
      disconnect(send_panel, SIGNAL(file_added()),
                 this, SLOT(_on_transaction_ready()));
      disconnect(send_panel, SIGNAL(peer_found()),
                 this, SLOT(_on_transaction_ready()));
    }

    void
    Onboarder::_send_onboarding_done()
    {
      auto* send_panel = this->_dock->_send_panel;
      disconnect(send_panel, SIGNAL(peer_found()),
                 this, SLOT(_on_peer_chosen()));
      disconnect(send_panel, SIGNAL(file_added()),
                 this, SLOT(_on_file_added_before_peer()));
      disconnect(send_panel, SIGNAL(sent()),
                 this, SLOT(_send_onboarding_done()));
      disconnect(send_panel, SIGNAL(canceled()),
                 this, SLOT(_send_onboarding_done()));
      emit sending_completed();
    }

    void
    Onboarder::_set_tooltip(QWidget* widget,
                            QString const& text,
                            Qt::AlignmentFlag alignment,
                            int duration)
    {
      ELLE_DEBUG_SCOPE("create new tooltip: %s on %s", text, *widget);
      this->_tooltip.reset(
        new fist::gui::Tooltip(
          text, widget, nullptr, alignment, duration));
      if (this->_dock->isHidden())
      {
        ELLE_DEBUG_SCOPE("dock is not visible, delay %s visibilty",
                         *this->_tooltip);
        this->_tooltip->hide();
      }
    }
  };
}
