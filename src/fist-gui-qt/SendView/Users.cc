#include <vector>

#include <QPainter>
#include <QHBoxLayout>

#include <elle/log.hh>
#include <elle/finally.hh>

#include <fist-gui-qt/SendView/Users.hh>
#include <fist-gui-qt/model/User.hh>
#include <fist-gui-qt/SearchResultWidget.hh>
#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.SendView.Users");

static int const margin = 7;

namespace
{
  static const QRegExp email_checker(regexp::email, Qt::CaseInsensitive);
}

namespace fist
{
  namespace sendview
  {
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
      , _loading_icon(new QMovie(QString(":/loading"), QByteArray(), this))
      , _icon(new QLabel(this))
      , _search_field(new SearchField(this))
      , _results()
      , _recipients()
      , _separator(new HorizontalSeparator(this, 10))
      , _users(new ListWidget(
                 this, ListWidget::Separator({QColor(0xF4, 0xF4, 0xF4)}, 10, 10)))
    {
      connect(this->_search_field, SIGNAL(up_pressed()), this->_users, SLOT(setFocus()));
      connect(this->_search_field, SIGNAL(down_pressed()), this->_users, SLOT(setFocus()));
      this->setContentsMargins(0, 0, 0, 0);
      this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
      connect(&this->_state, SIGNAL(results_ready()),
              this, SLOT(_set_users()));
      {
        this->_users->setMaxRows(3);
      }
      {
        this->set_icon(this->_magnifier);
      }
      {
        this->_loading_icon->start();
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
          view::send::search_field::style(*this->_search_field);
          this->_search_field->setContentsMargins(margin, 0, margin, 0);
          this->_search_field->setPlaceholderText(view::send::search_field::text);
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

      // Search delay.
      {
        this->_search_delay.setSingleShot(true);
        connect(&this->_search_delay, SIGNAL(timeout()),
                this, SLOT(delay_expired()));
      }

      this->text_changed("");
    }

    void
    Users::clear_search()
    {
      this->_search_field->setText("");
      this->text_changed("");
    }

    void
    Users::clear()
    {
      this->_recipients.clear();
      this->_search_field->clear();
      this->clear_results();
    }

    void
    Users::clear_results()
    {
      this->_results.clear();
      this->_users->clearWidgets();
    }

    bool
    Users::peer_valid() const
    {
      return !this->_recipients.empty() ||
        email_checker.exactMatch(this->text());
    }

    void
    Users::_select_first_user()
    {
      if (!this->_results.empty())
      {
        // XXX: This suxx so much because results is an unordered_map so we are
        // not supposed to make any assumption on the order of the elements.
        // But, it works so while the swagg is not returned, keep that solution.
        auto it = this->_results.begin();
        std::advance(it, this->_results.size() - 1);
        it->second->trigger();
      }
    }

    void
    Users::_set_users()
    {
      ELLE_TRACE_SCOPE("got result from future");
      elle::SafeFinally restore_magnifier(
        [&] { this->set_icon(this->_magnifier); });
      this->set_users(this->_state.results(), false);
    }

    void
    Users::set_users(UserList const& users, bool local)
    {
      ELLE_TRACE_SCOPE("%s: set users", *this);
      for (auto id: users)
      {
        auto const& model = this->_state.user(id);
        ELLE_DEBUG("-- %s", model);
        if (this->_results.find(model.id()) == this->_results.end())
        {
          bool picked = this->_recipients.find(model.id()) != this->_recipients.end();
          auto widget = std::make_shared<SearchResultWidget>(model, picked, this);
          connect(widget.get(),
                  SIGNAL(selected(uint32_t)),
                  this,
                  SLOT(_add_peer(uint32_t)));
          connect(widget.get(),
                  SIGNAL(unselected(uint32_t)),
                  this,
                  SLOT(_remove_peer(uint32_t)));
          this->_users->add_widget(widget, ListWidget::Position::Bottom);
          // local ? ListWidget::Position::Top : ListWidget::Position::Bottom);
          this->_results[model.id()] = widget;
        }
      }

      if (this->_users->widgets().isEmpty() && !local)
      {
        if (!email_checker.exactMatch(this->text()))
        {
          this->_users->add_widget(
            std::make_shared<TextListItem>(
              "<b>No result</b><br />Send to an email address instead", 60, this),
            ListWidget::Position::Top);
        }
        else
        {
          emit peer_found();
        }
      }

      if (!this->_users->widgets().isEmpty())
        this->_separator->show();
      else
        this->_separator->hide();
    }

    /*------.
    | Slots |
    `------*/
    void
    Users::_add_peer(uint32_t uid)
    {
      ELLE_TRACE_SCOPE("%s: add peer: %s", *this, uid);
      auto index = [&] {
        try
        {
          return this->_users->index(this->_results.at(uid));
        }
        catch (std::out_of_range const&)
        {
          return -1;
        }
      }();
      emit send_metric(UIMetrics_SelectPeer,
                       {
                         { "filter", this->text().toStdString() },
                         { "index", std::to_string(index) },
                       });
      this->_recipients.insert(uid);
      emit peer_found();
    }

    void
    Users::_remove_peer(uint32_t uid)
    {
      ELLE_TRACE_SCOPE("%s: remove peer: %s", *this, uid);
      auto index = [&] {
        try
        {
          return this->_users->index(this->_results.at(uid));
        }
        catch (std::out_of_range const&)
        {
          return -1;
        }
      }();
      emit send_metric(UIMetrics_UnselectPeer,
                       {
                         { "filter", this->text().toStdString() },
                         { "index", std::to_string(index) },
                       });
      this->_recipients.erase(uid);
    }

    void
    Users::set_icon(QPixmap const& pixmap)
    {
      this->_icon->show();
      this->_icon->setPixmap(pixmap);
      this->update();
    }

    void
    Users::set_icon(QMovie& movie)
    {
      this->_icon->show();
      this->_icon->setMovie(&movie);
      this->_icon->movie()->start();
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
      return this->_search_field->text();
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
    }

    void
    Users::text_changed(QString const& text)
    {
      this->clear_results();

      if (text.isEmpty())
        this->set_users(this->_state.swaggers(), true);
      else
        this->set_users(this->_state.swaggers(text), true);

      if (!text.isEmpty())
        this->_search_delay.start(300);
      else
        this->delay_expired();
    }

    void
    Users::delay_expired()
    {
      auto search = this->_search_field->text();

      ELLE_TRACE_SCOPE("%s: search changed: %s", *this, search);

      QString trimmed_search = search.trimmed();
      if (trimmed_search.size() != 0)
      {
        this->set_icon(*this->_loading_icon);
      }
      this->_state.search(trimmed_search);
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
