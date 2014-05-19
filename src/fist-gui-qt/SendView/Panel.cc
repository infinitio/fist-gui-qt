#include <iostream>

#include <QDir>
#include <QFuture>
#include <QtConcurrentRun>
#include <QTabWidget>

#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/log.hh>

#include <surface/gap/gap.hh>

#include <fist-gui-qt/AvatarIcon.hh>
#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/SearchResultWidget.hh>
#include <fist-gui-qt/SendView/Files.hh>
#include <fist-gui-qt/SendView/Panel.hh>
#include <fist-gui-qt/SendView/Users.hh>
#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>

/*-------------.
| Construction |
`-------------*/

ELLE_LOG_COMPONENT("infinit.FIST.SendView.Panel");

namespace
{
  static const QRegExp email_checker(regexp::email,
                                     Qt::CaseInsensitive);
}

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
    {
      this->_tabs->add_tab("SEND TO USER", { this->_users, this->_message, this->_file_adder, this->_message->top_separator(), });
      this->_tabs->add_tab("GET A LINK", { this->_message, this->_file_adder, });

      this->footer()->setParent(this);

      connect(this->_file_adder, SIGNAL(clicked()),
              this, SIGNAL(choose_files()));

      connect(this->_file_adder->attach(), SIGNAL(released()),
              this, SIGNAL(choose_files()));

      connect(this->_file_adder->add_file(), SIGNAL(released()),
              this, SIGNAL(choose_files()));

      connect(this->_file_adder, SIGNAL(file_dropped(QUrl const&)),
              this->_file_adder, SLOT(add_file(QUrl const&)));

      connect(this->footer()->send(), SIGNAL(clicked()),
              this, SLOT(_send()));

      connect(this->footer()->back(), SIGNAL(clicked()),
              this, SIGNAL(canceled()));

      connect(this, SIGNAL(drag_entered()),
              this->_file_adder, SLOT(on_entered()));

      connect(this, SIGNAL(drag_left()),
              this->_file_adder, SLOT(on_left()));

      connect(this, SIGNAL(sent()),
              this, SIGNAL(switch_signal()));

      connect(this, SIGNAL(canceled()),
              this, SIGNAL(switch_signal()));

      this->setAcceptDrops(true);
    }

    void
    Panel::_pick_user()
    {
      ELLE_TRACE_SCOPE("%s: pick user", *this);
      if (!this->_users->recipients().empty())
      {
        // ELLE_DEBUG("try to send to %s", this->_peer_id);
        this->_send();
      }
      else
      {
        this->_users->search_field()->setFocus();
      }
    }

    void
    Panel::_send()
    {
      ELLE_TRACE_SCOPE("%s: send", *this);

      if (!this->_users->peer_valid())
      {
        ELLE_DEBUG("peer is not set");
        this->_users->search_field()->setFocus();
        return;
      }

      if (this->_file_adder->files().empty())
      {
        ELLE_DEBUG("file list is empty");
        this->_file_adder->pulse();
        return;
      }

      char** filenames;
      if ((filenames = (char**)malloc((this->_file_adder->files().size() + 1) * sizeof(char*)))
          == nullptr)
      {
        ELLE_ERR("unable to allocate file list");
      }

      for (int i = 0; i < this->_file_adder->files().size(); i++)
      {
        auto filepath =
          QDir::toNativeSeparators(this->_file_adder->files().keys().at(i).toLocalFile()).toStdString();

        ELLE_DEBUG("file to add: %s", filepath);

        if ((filenames[i] = (char*)malloc((filepath.size() + 1))) == nullptr)
        {
          ELLE_ERR("unable to allocate file name");
        }

        strcpy(filenames[i], filepath.c_str());
      }

      filenames[this->_file_adder->files().size()] = nullptr;

      std::string message = this->_message->text().toStdString();
      for (auto const& recipient: this->_users->recipients())
      {
        if (recipient != gap_null())
        {
          gap_send_files(
            this->_state.state(), recipient, filenames, message.c_str());
        }
      }
      if (email_checker.exactMatch(this->_users->text()))
      {
        QString recipient = this->_users->text();
        ELLE_TRACE_SCOPE("send files to %s", this->_users->text());
        gap_send_files_by_email(
          this->_state.state(), recipient.toStdString().c_str(), filenames, message.c_str());
      }

      auto** cpy = filenames;
      while (*cpy != nullptr)
      {
        ::free((void*) *cpy);
        ++cpy;
      }
      ::free((void*) filenames);

      emit sent();
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
        this->_file_adder->add_file()->click();
    }

  /*------------.
  | Show & Hide |
  `------------*/
    void
    Panel::on_show()
    {
      ELLE_TRACE_SCOPE("%s: show", *this);
      emit set_background_color(Qt::white);
      emit shown();
      this->update();
      this->_tabs->activate_first();
      this->_users->clear_search();
      this->_users->setFocus();
    }

    void
    Panel::on_hide()
    {
      ELLE_TRACE_SCOPE("%s: hide", *this);
      emit set_background_color(view::background);

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

      if (event->mimeData()->hasUrls())
        for (auto const& url: event->mimeData()->urls())
          if (url.isLocalFile())
          {
            event->acceptProposedAction();
            this->_file_adder->add_file(url);
          }
    }

    void
    Panel::focusInEvent(QFocusEvent* event)
    {
      // this->_users->setFocus();
      Super::focusInEvent(event);
    }
  }
}
