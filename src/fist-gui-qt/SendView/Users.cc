#include <vector>
#include <elle/container/set.hh>
#include <algorithm>

#include <boost/functional/hash.hpp>

#include <QtCore>
#include <QPainter>
#include <QHBoxLayout>
#include <QScrollArea>

#include <elle/log.hh>
#include <elle/finally.hh>

#include <fist-gui-qt/SendView/OwnDeviceSearchResult.hh>
#include <fist-gui-qt/SendView/SearchResultWidget.hh>
#include <fist-gui-qt/SendView/NoSearchResultWidget.hh>
#include <fist-gui-qt/SendView/EmailResultWidget.hh>
#include <fist-gui-qt/SendView/Users.hh>
#include <fist-gui-qt/SendView/ui.hh>
#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/model/User.hh>
#include <fist-gui-qt/regexp.hh>

ELLE_LOG_COMPONENT("infinit.FIST.SendView.Users");

static int const margin = 7;

namespace std
{
  std::size_t
  hash<fist::sendview::Recipient>::operator()(
    fist::sendview::Recipient const& recipient) const
  {
    // XXX: The id is definitely not unique.
    auto id = recipient.id();
    if (id == gap_null())
      id = recipient.to_email()
        ? std::hash<std::string>()(recipient.email().get().toStdString())
        : 0;
    auto k = recipient.to_device()
      ? std::hash<std::string>()(recipient.device().get().id().toStdString())
      : 0;
    size_t seed = 0;
    boost::hash_combine(seed, id);
    boost::hash_combine(seed, k);
    return seed;
  }
}

namespace fist
{
  namespace sendview
  {

    bool
    Recipient::operator == (uint32_t id) const
    {
      ELLE_DEBUG_SCOPE("%s: compare recipient by id: %s", *this, id);
      return this->id() == id;
    }

    bool
    Recipient::operator == (QString const& value) const
    {
      ELLE_DEBUG_SCOPE("%s: compare recipient by string: %s", *this, value);
      if (this->to_device())
        return this->_device.get().id() == value;
      if (this->to_email())
        return this->_email.get() == value;
      return false;
    }

    bool
    Recipient::operator == (Recipient const& rec) const
    {
      ELLE_DEBUG_SCOPE("compare %s and %s", *this, rec);
      if (this->to_email())
      {
        if (!rec.to_email())
          return false;
        return this->_email.get() == rec.email().get();
      }
      if (this->to_device())
      {
        if (!rec.to_device())
          return false;
        return this->_device.get().id() == rec.device().get().id();
      }
      return this->id() == rec.id();
    }


    SearchField::SearchField(QWidget* parent)
      : QLineEdit(parent)
    {}

    SearchField::SearchField(QString const& text, QWidget* parent)
      : QLineEdit(text, parent)
    {}

    void
    SearchField::keyPressEvent(QKeyEvent* event)
    {
      ELLE_DEBUG("key pressed: %s", event->key())
        if (event->key() == Qt::Key_Up)
          emit up_pressed();
        else if (event->key() == Qt::Key_Down)
          emit down_pressed();
        else if (event->key() == Qt::Key_Escape)
        {
          static_cast<QWidget*>(this->parent())->setFocus(Qt::OtherFocusReason); return;
        }
        else if (this->text().isEmpty() && (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right))
        {
          this->parent()->event(event); return;
        }
        else
          QLineEdit::keyPressEvent(event);
      event->accept();
    }

    void
    SearchField::focusInEvent(QFocusEvent* event)
    {
      Super::focusInEvent(event);
      if (this->text().isEmpty())
        this->setPlaceholderText(this->placeholderText());
    }

    Users::Users(fist::State& state,
                 QWidget* owner)
      :  QWidget(owner)
      , _state(state)
      , _magnifier(":/send/search")
      , _icon(new QLabel(this))
      , _search_field(new SearchField(this))
      , _results()
      , _recipients()
      , _separator(new HorizontalSeparator(this, 0))
      , _users(
        new ListWidget(
          this, ListWidget::Separator({QColor(0xFE, 0xFE, 0xFE)}, 45, 15),
          QColor{0xFE, 0xFE, 0xFE}))
      , _max(20)
    {
      this->setContentsMargins(0, 0, 0, 0);
      this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
      {
        this->_search_field->installEventFilter(this);
        connect(this->_search_field, SIGNAL(up_pressed()),
                this->_users, SLOT(setFocus()));
        connect(this->_search_field, SIGNAL(down_pressed()),
                this->_users, SLOT(setFocus()));
      }
      {
        this->set_icon(this->_magnifier);
      }
      {
        this->_users->setMaxRows(4);
      }
      auto* vlayout = new QVBoxLayout(this);
      vlayout->setAlignment(Qt::AlignVCenter);
      vlayout->setContentsMargins(0, 0, 0, 0);
      vlayout->setSpacing(0);
      {
        auto* layout = new QHBoxLayout;
        layout->setSpacing(12 - margin);
        layout->setContentsMargins(18, 4, 5, 4);
        // Icon.
        {
          layout->addWidget(this->_icon);
        }
        // Search field.
        {
          this->_search_field->setFrame(false);
          view::search_field::style(*this->_search_field);
          this->_search_field->setContentsMargins(margin, 0, margin, 0);
          this->_search_field->setPlaceholderText(view::search_field::text);
          this->_search_field->setFixedHeight(this->height());
          this->_search_field->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
          connect(this->_search_field, SIGNAL(textChanged(QString const&)),
                  this, SLOT(text_changed(QString const&)));
          connect(this->_search_field, SIGNAL(returnPressed()),
                  this, SLOT(_select_first_user()));
          layout->addWidget(this->_search_field, 1);
        }
        vlayout->addLayout(layout);
      }
      vlayout->addWidget(this->_separator);
      vlayout->addWidget(this->_users);
    }

    bool
    Users::eventFilter(QObject *obj, QEvent *event)
    {
      if (obj == this->_search_field)
      {
        if (event->type() == QEvent::FocusIn)
          emit search_field_focused();
        else if (event->type() == QEvent::FocusOut)
          emit search_field_unfocused();
      }
      return Super::eventFilter(obj, event);
    }

    void
    Users::clear_search(bool final)
    {
      ELLE_DEBUG_SCOPE("%s: clear search", *this);
      this->set_text("");
      if (!final)
        this->text_changed("");
    }

    void
    Users::clear(bool final)
    {
      ELLE_DEBUG_SCOPE("%s: clear", *this);
      this->_recipients.clear();
      this->clear_search(final);
      this->clear_results();
    }

    void
    Users::clear_results()
    {
      ELLE_DEBUG_SCOPE("%s: clear results", *this);
      this->_results.clear();
      this->_users->clearWidgets();
    }

    bool
    Users::peer_valid() const
    {
      return !this->_recipients.empty();
    }

    void
    Users::_select_first_user()
    {
      ELLE_DEBUG_SCOPE("%s: select first user", *this);
      if (!this->_recipients.empty() && this->text().isEmpty())
      {
        ELLE_DEBUG("remove recipient")
          this->_remove_recipient(*this->_recipients.begin());
      }
      else if (!this->_results.empty())
      {
        ELLE_DEBUG("add recipient")
          this->_add_recipient(*this->_results.begin());
      }
    }

    void
    Users::_add_result(Recipient const& rec,
                       bool foo)
    {
      ELLE_DEBUG_SCOPE("add result: %s", rec);
      if (rec.id() == this->_state.my_id() && rec.to_device())
      {
        auto widget = std::make_shared<OwnDeviceSearchResult>(
          this->_state.me(), rec.device().get(), foo, this);
        connect(widget.get(), SIGNAL(selected_device(uint32_t, QString const&)),
                this, SLOT(_add_device(uint32_t, QString const&)));
        connect(widget.get(), SIGNAL(unselected_device(uint32_t, QString const&)),
                this, SLOT(_remove_device(uint32_t, QString const&)));
        this->_users->add_widget(widget, ListWidget::Position::Bottom);
      }
      else if (rec.to_email() && rec.id() != this->_state.my_id())
      {
        auto widget = std::make_shared<EmailResultWidget>(
          rec.email().get(), foo, this);
        connect(widget.get(), SIGNAL(email_selected(QString const&)),
                this, SLOT(_add_email(QString const&)));
        connect(widget.get(), SIGNAL(email_unselected(QString const&)),
                this, SLOT(_remove_email(QString const&)));
        this->_users->add_widget(widget, ListWidget::Position::Bottom);
        this->_results.insert(this->_results.begin(), rec);
      }
      else
      {
        auto widget = std::make_shared<SearchResultWidget>(
          this->_state.user(rec.id()), foo, this);
        connect(widget.get(), SIGNAL(selected(uint32_t)),
                this, SLOT(_add_peer(uint32_t)));
        connect(widget.get(), SIGNAL(unselected(uint32_t)),
                this, SLOT(_remove_peer(uint32_t)));
        this->_users->add_widget(widget, ListWidget::Position::Bottom);
      }
    }

    void
    Users::_compute_results(UserList const& users,
                            bool no_self)
    {
      ELLE_DEBUG_SCOPE("%s: compute_results from %s", *this, users);
      ELLE_DUMP("recipients: %s", this->_recipients);
      this->_results.clear();
      for (auto const& user: users)
      {
        if (std::find_if(this->_recipients.begin(),
                         this->_recipients.end(),
                         [&] (Recipient const& recipient)
                         {
                           return recipient.id() == user;
                         }) == this->_recipients.end())
        {
          ELLE_DEBUG("%s not in the recipient list", user);
          if (user != this->_state.me().id())
            this->_results.push_back(Recipient(user));
          else if (!no_self)
            this->_results.insert(this->_results.begin(), Recipient(user));
        }
      }
    }

    void
    Users::set_users(UserList const& users, bool local)
    {
      this->_compute_results(users, this->text().isEmpty());
      if (regexp::email::checker.exactMatch(this->text()))
      {
        ELLE_DEBUG("text %s is an email", this->text());
        auto picked = std::find_if(
          this->_recipients.begin(),
          this->_recipients.end(),
          [&] (Recipient const& recipient)
          {
            return recipient == this->text();
          }) != this->_recipients.end();
        this->_add_result(Recipient(gap_null(), boost::none, this->text()), picked);
      }
      else
      {
        if (this->text().isEmpty())
        {
          ELLE_DEBUG("recipients")
            for (Recipient const& rec: this->_recipients)
            {
              ELLE_DEBUG("add recipient %s", rec);
              this->_add_result(rec, true);
            };
          if (!this->_recipients.empty())
          {
            this->_users->add_separator();
          }
          auto devices = this->_state.devices();
          if (devices.size() > 1)
          {
            int i = 0;
            ELLE_DEBUG("devices")
              for (auto device: devices)
              {
                ELLE_DEBUG("device: %s", device);
                auto recipient_device = Recipient(this->_state.my_id(), device);
                if (this->_state.device().id() != device.id() &&
                    std::find_if(this->_recipients.begin(),
                                 this->_recipients.end(),
                                 [&] (Recipient const& recipient)
                                 {
                                   ELLE_DUMP("%s - %s", recipient,  device.id());
                                   return recipient == device.id();
                                 }) == this->_recipients.end())
                {
                  auto it = this->_results.begin();
                  std::advance(it, i);
                  ELLE_DEBUG("add device %s", *it);
                  this->_results.insert(it, recipient_device);
                  this->_add_result(recipient_device, false);
                  ++i;
                }
              }
          }
          else
          {
            this->_add_result(Recipient(this->_state.my_id()), false);
          }
          ELLE_DEBUG("%s", this->_results);
          if (!this->_results.empty())
            this->_users->add_separator();
        }

        int count = 0;
        ELLE_DEBUG("results")
          for (Recipient const& rec: this->_results)
          {
            ELLE_DEBUG("result: %s", rec);
            if (rec.to_device() || rec.to_email())
              continue;
            this->_add_result(rec, false);
            ++count;
            if (count == this->_max)
            {
              break;
            }
          }
      }

      if (this->_results.empty() && !this->text().isEmpty() && !regexp::email::checker.exactMatch(this->text()))
      {
        if (this->_users->widgets().empty())
          this->_users->add_widget(
            std::make_shared<NoSearchResultWidget>(this),
            ListWidget::Position::Top);
      }

    }

    bool
    Users::_add_recipient(Recipient const& recipient)
    {
      ELLE_DEBUG_SCOPE("add recipient %s", recipient);
      auto size = this->_recipients.size();
      this->_recipients.insert(this->_recipients.begin(), recipient);
      std::sort(this->_recipients.begin(), this->_recipients.end());
      this->_recipients.erase(
        std::unique(this->_recipients.begin(),
                    this->_recipients.end()),
        this->_recipients.end());
      bool had_effect = this->_recipients.size() != size;
      if (had_effect)
        this->clear_search();
      return had_effect;
    }

    bool
    Users::_remove_recipient(Recipient const& recipient)
    {
      ELLE_DEBUG_SCOPE("add remove %s", recipient);
      auto size = this->_recipients.size();
      auto it = std::find_if(this->_recipients.begin(),
                             this->_recipients.end(),
                             [&] (Recipient const& rec)
                             {
                               return recipient == rec;
                             });
      if (it != this->_recipients.end())
        this->_recipients.erase(it);
      bool had_effect = this->_recipients.size() != size;
      if (had_effect)
        this->clear_search();
      return had_effect;
    }

    /*------.
    | Slots |
    `------*/
    void
    Users::_add_peer(uint32_t uid)
    {
      ELLE_TRACE_SCOPE("%s: add peer: %s", *this, uid);
      this->_add_recipient(Recipient(uid));
    }

    void
    Users::_remove_peer(uint32_t uid)
    {
      ELLE_TRACE_SCOPE("%s: remove peer: %s", *this, uid);
      this->_remove_recipient(Recipient(uid));
    }

    void
    Users::_add_device(uint32_t uid,
                       QString const& device_id)
    {
      ELLE_DEBUG_SCOPE("%s: add device %s", *this, device_id);
      this->_add_recipient(Recipient{this->_state.my_id(), this->_state.device(device_id)});
    }

    void
    Users::_remove_device(uint32_t uid,
                          QString const& device_id)
    {
      ELLE_DEBUG_SCOPE("%s: remove device %s", *this, device_id);
      this->_remove_recipient(
        Recipient{this->_state.my_id(), this->_state.device(device_id)});
    }

    void
    Users::_add_email(QString const& email)
    {
      ELLE_DEBUG_SCOPE("%s: add email %s", *this, email);
      this->_add_recipient(Recipient{gap_null(), boost::none, email});
    }

    void
    Users::_remove_email(QString const& email)
    {
      ELLE_DEBUG_SCOPE("%s: remove email %s", *this, email);
      this->_remove_recipient(Recipient{gap_null(), boost::none, email});
    }

    void
    Users::set_icon(QPixmap const& pixmap)
    {
      this->_icon->show();
      this->_icon->setPixmap(pixmap);
      this->update();
    }

    void
    Users::set_text(QString const& text)
    {
      ELLE_TRACE_SCOPE("%s: set text: %s", *this, text);
      disconnect(this->_search_field, SIGNAL(textChanged(QString const&)),
                 this, SLOT(text_changed(QString const&)));
      this->_search_field->setText(text);
      connect(this->_search_field, SIGNAL(textChanged(QString const&)),
              this, SLOT(text_changed(QString const&)));
    }

    QString
    Users::text() const
    {
      return this->_search_field->text().trimmed();
    }

    void
    Users::keyPressEvent(QKeyEvent* event)
    {
      if (event->key() == Qt::Key_Up)
      {
        emit up_pressed(); return;
      }
      else if (event->key() == Qt::Key_Down)
      {
        emit down_pressed(); return;
      }
      else if (event->key() == Qt::Key_Return && !event->isAccepted())
      {
        emit return_pressed(); return;
      }
      Super::keyPressEvent(event);
    }

    void
    Users::showEvent(QShowEvent* event)
    {
      this->_search_field->setFocus();
      this->clear_search();
    }

    void
    Users::hideEvent(QHideEvent* event)
    {
      ELLE_DEBUG_SCOPE("%s: hide event", *this);
      this->_search_field->setText("");
      this->_state.cancel_search();
      Super::hideEvent(event);
    }

    void
    Users::text_changed(QString const& text)
    {
      ELLE_DEBUG_SCOPE("%s: text changed to %s", *this, text);
      QString trimmed_search = text.trimmed();
      auto results = this->_state.search(trimmed_search);
      if (!results.empty() || !this->_results.empty() ||
          !regexp::email::checker.exactMatch(this->text()))
        this->clear_results();
      this->set_users(results, true);
    }

    void
    Users::focusInEvent(QFocusEvent* event)
    {
      if (event->reason() != Qt::OtherFocusReason)
        this->_search_field->setFocus();
    }

    QSize
    Users::sizeHint() const
    {
      return QSize(320, Super::sizeHint().height());
    }

    QSize
    Users::minimumSizeHint() const
    {
      return this->sizeHint();
    }

  }
}
