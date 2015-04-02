#include <iostream>

#include <QDir>
#include <QFuture>
#include <QtConcurrentRun>
#include <QTabWidget>

#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/container/set.hh>
#include <elle/log.hh>

#include <surface/gap/gap.hh>

#include <fist-gui-qt/AvatarIcon.hh>
#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/SendView/Files.hh>
#include <fist-gui-qt/SendView/Panel.hh>
#include <fist-gui-qt/SendView/Users.hh>
#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/regexp.hh>
#include <fist-gui-qt/utils.hh>

/*-------------.
| Construction |
`-------------*/

ELLE_LOG_COMPONENT("infinit.FIST.SendView.Panel");

namespace fist
{
  namespace sendview
  {
    Panel::Panel(fist::State& state)
      : Super(new Footer)
      , _state(state)
      , _tabs(new fist::gui::TabWidget(this))
      , _users(new Users(_state, this))
      , _message(new Message(this))
      , _file_adder(new Files(this))
      , _transaction_tab(
        this->_tabs->add_tab(
          "SEND TO SOMEONE",
          {
            this->_users,
            this->_message,
            this->_file_adder,
            this->_message->top_separator(),
          }))
      , _link_tab(
        this->_tabs->add_tab(
          "GET A LINK",
          {
            this->_message,
            this->_file_adder,
          }))
    {
      // File adder.
      connect(this->_file_adder, SIGNAL(clicked()),
              this, SIGNAL(choose_files()));
      connect(this->_file_adder, SIGNAL(dropped()),
              this, SLOT(_dropped()));
      // Footer.
      connect(this->footer()->send(), SIGNAL(clicked()), this, SLOT(_send()));
      connect(this->footer()->back(), SIGNAL(clicked()),
              this, SIGNAL(canceled()));
      // Cancel.
      connect(this, SIGNAL(canceled()), this, SIGNAL(switch_signal()));
      // Sent.
      connect(this, SIGNAL(sent()), this, SIGNAL(switch_signal()));
      connect(this, SIGNAL(sent()), this, SLOT(hide()));
      // Tabs.
      connect(this->_link_tab, SIGNAL(activated()), this, SLOT(link_mode()));
      connect(this->_transaction_tab, SIGNAL(activated()),
              this, SLOT(p2p_mode()));
      // Metrics.
      connect(this->_users, SIGNAL(send_metric(UIMetricsType, std::unordered_map<std::string, std::string> const&)),
              &this->_state, SLOT(send_metric(UIMetricsType, std::unordered_map<std::string, std::string> const&)));

      this->setAcceptDrops(true);

      this->setMaximumHeight(500);
    }

    void
    Panel::_mode_implementation()
    {
      switch (this->mode())
      {
        case Mode::p2p:
          this->_transaction_tab->click();
          break;
        case Mode::link:
          this->_link_tab->click();
          break;
      }
      this->footer()->mode(this->mode());
    }

    void
    Panel::p2p_mode()
    {
      this->mode(Mode::p2p);
    }

    void
    Panel::link_mode()
    {
      this->mode(Mode::link);
    }

    void
    Panel::_pick_user()
    {
      ELLE_TRACE_SCOPE("%s: pick user", *this);
      if (!this->_users->recipients().empty())
      {
        this->_send();
      }
      else
      {
        this->_users->search_field()->setFocus();
      }
    }

    bool
    Panel::_check_files()
    {
      if (this->_file_adder->files().empty())
      {
        ELLE_DEBUG("file list is empty");
      }

      return !this->_file_adder->files().empty();
    }

    void
    Panel::_send()
    {
      if (!this->_check_files())
        return;

      if (this->_tabs->is_active_tab(*this->_transaction_tab) && !this->_users->peer_valid())
      {
        ELLE_DEBUG("peer is not set");
        this->_users->search_field()->setFocus();
        return;
      }

      std::vector<std::string> files;
      for (int i = 0; i < this->_file_adder->files().size(); ++i)
      {
        files.push_back(
          QFile_to_string(this->_file_adder->files().keys().at(i));
      }

      auto message_array = this->_message->text().toUtf8();
      std::string message = std::string(message_array.constData());

      ELLE_DEBUG("message: %s", message);

      if (this->_tabs->is_active_tab(*this->_link_tab))
      {
        ELLE_TRACE("generate link")
          gap_create_link_transaction(this->_state.state(), files, message.c_str());
      }
      else
      {
        auto recipients = this->_users->recipients();
        if (regexp::email::checker.exactMatch(this->_users->text()))
        {
          QString recipient = this->_users->text();
          uint32_t id = this->_state.user_id(recipient.toStdString());
          if (id == gap_null())
          {
            ELLE_TRACE_SCOPE("send files to %s", recipient);
            gap_send_files(
              this->_state.state(), recipient.toStdString(), files, message);
          }
          else if (recipients.find(id) == recipients.end())
            recipients.insert(id);
        }
        for (auto const& recipient: recipients)
        {
          if (recipient.id() != gap_null())
          {
            if (recipient.id() != this->_state.my_id())
              ELLE_ASSERT(!recipient.device_uuid());
            gap_send_files(
              this->_state.state(), recipient, files, message, recipient.device_uuid());
          }
        }
      }
      ELLE_DEBUG("done")
        emit sent();
    }

    void
    Panel::_dropped()
    {
      this->_state.send_metric(UIMetrics_AddFilesDropOnSendView);
    }

    void
    Panel::_canceled()
    {
      this->_state.send_metric(UIMetrics_SendTrash);
      emit switch_signal();
    }

    void
    Panel::send()
    {
      this->_send();
    }

    void
    Panel::avatar_available(uint32_t uid)
    {
      ELLE_TRACE_SCOPE("%s: user (%s) avatar available", *this, uid);
      auto it = this->_user_models.find(uid);
      if (it != this->_user_models.end())
      {
        it->second->avatar_available();
      }
    }

    void
    Panel::keyPressEvent(QKeyEvent* event)
    {
      ELLE_DEBUG_SCOPE("key pressed: %s", event->key());
      if (event->key() == Qt::Key_Escape)
        emit canceled();
      else if (event->key() == Qt::Key_Return)
        this->_pick_user();
      else if (event->key() == Qt::Key_Right)
        this->_tabs->activate_next();
      else if (event->key() == Qt::Key_Left)
        this->_tabs->activate_previous();
      else if (event->key() == Qt::Key_F)
        emit choose_files();
    }

  /*------------.
  | Show & Hide |
  `------------*/

    void
    Panel::showEvent(QShowEvent* event)
    {
      ELLE_TRACE_SCOPE("%s: show", *this);
      emit shown();
      this->update();
      this->_tabs->activate_first();
      this->_users->clear_search();
      this->_users->setFocus(Qt::ActiveWindowFocusReason);
      this->updateGeometry();
      Super::showEvent(event);
    }

    void
    Panel::hideEvent(QHideEvent* event)
    {
      ELLE_TRACE_SCOPE("%s: hide", *this);
      this->clear();
      Super::hideEvent(event);
    }

    void
    Panel::clear()
    {
      this->_users->clear();
      this->_file_adder->clear();
      this->_message->clear();
    }

  /*-------.
  | Footer |
  `-------*/
    Footer*
    Panel::footer()
    {
      return static_cast<Footer*>(this->_footer);
    }

    void
    Panel::print(std::ostream& stream) const
    {
      stream << "Panel";
    }

    void
    Panel::dragEnterEvent(QDragEnterEvent *event)
    {
      ELLE_TRACE_SCOPE("%s: drag entered", *this);

      if (event->mimeData()->hasUrls())
        for (auto const& url: event->mimeData()->urls())
          if (url.isLocalFile())
          {
            event->acceptProposedAction();
            emit drag_entered();
            return;
          }
    }

    void
    Panel::dragLeaveEvent(QDragLeaveEvent *event)
    {
      ELLE_TRACE_SCOPE("%s: drag left", *this);
      emit drag_left();
    }

    void
    Panel::dropEvent(QDropEvent *event)
    {
      ELLE_TRACE_SCOPE("%s: drop", *this);

      QList<QUrl> files;
      if (event->mimeData()->hasUrls())
        for (auto const& url: event->mimeData()->urls())
          if (url.isLocalFile())
          {
            event->acceptProposedAction();
            files.append(url);
          }
      this->_dropped();
      this->_file_adder->add_files(files);
    }

    void
    Panel::focusInEvent(QFocusEvent* event)
    {
      // this->_users->setFocus();
      Super::focusInEvent(event);
    }
  }
}
