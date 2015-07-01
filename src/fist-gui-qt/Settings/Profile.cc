#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/Settings/Profile.hh>
#include <fist-gui-qt/Settings/utils.hh>
#include <fist-gui-qt/State.hh>

#include <QFileDialog>
#include <QGridLayout>

namespace fist
{
  namespace prefs
  {
    Profile::Profile(fist::State& state,
                     QWidget *parent)
      : QWidget(parent)
      , _state(state)
      , _avatar(new AvatarIcon(this->_state.me().avatar(), QSize(64, 64), this))
      , _fullname(line_edit(this->_state.me().fullname(), this))
      , _handle(line_edit(this->_state.me().handle(), this))
    {
      connect(&this->_state.me(), SIGNAL(avatar_updated()),
              this, SLOT(_on_avatar_updated()));
      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::Base, Qt::white);
      }
      this->setPalette(palette);
      {
        connect(this->_fullname, SIGNAL(editingFinished()),
                this, SLOT(_update_fullname()));
        connect(this->_handle, SIGNAL(editingFinished()),
                this, SLOT(_update_handle()));
      }
      this->setFont(view::section::style.font());
      this->setFocusPolicy(Qt::NoFocus);
      QGridLayout* layout = new QGridLayout(this);
      layout->setSpacing(25);
      layout->setContentsMargins(45, 45, 45, 45);
      layout->setColumnStretch(4, 1);
      layout->addItem(new QSpacerItem(45, 0, QSizePolicy::Fixed), 0, 1, -1);
      {
        auto* account = new QLabel("Account", this);
        view::title::style(*account);
        layout->addWidget(account, 0, 0); // , 1, 0);
      }
      layout->addWidget(section("Avatar", this), 1, 0, Qt::AlignTop | Qt::AlignLeft);
      layout->addWidget(this->_avatar, 1, 2);
      {
        auto* button = make_button("Change", this);
        connect(button, SIGNAL(released()), this, SLOT(_change_avatar()));
        layout->addWidget(button, 1, 3, 1, -1);
      }
      layout->addWidget(section("Name", this), 2, 0, Qt::AlignTop | Qt::AlignLeft);
      layout->addWidget(this->_fullname, 2, 2, 1, -1);
      layout->addWidget(section("Handle", this), 3, 0, Qt::AlignTop | Qt::AlignLeft);
      layout->addWidget(this->_handle, 3, 2, 1, -1);
      layout->addWidget(section("Email", this), 4, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->setSpacing(5);
        vlayout->addWidget(new QLabel(this->_state.me().email(), this));
        vlayout->addWidget(new QLabel("You can have multiple email addresses", this));
        vlayout->addWidget(
          link(view::manage_email_addresses::text.arg(
                 QString(QUrl::toPercentEncoding(this->_state.web_login_token())),
                 this->_state.me().emails()[0]),
               this));
        layout->addLayout(vlayout, 4, 2, 1, -1);
      }
      layout->addWidget(section("Password", this), 5, 0, Qt::AlignTop | Qt::AlignLeft);
      {
        auto* button = make_button("Change", this);
        connect(button, SIGNAL(released()), this, SLOT(_change_password()));
        layout->addWidget(button, 5, 2, 1, -1);
      }
      layout->addWidget(
        link(view::account::text.arg(
               QString(QUrl::toPercentEncoding(this->_state.web_login_token())),
               this->_state.me().emails()[0]),
             this));

    }

    void
    Profile::_on_avatar_updated()
    {
      this->_avatar->set_avatar(this->_state.me().avatar());
    }

    void
    Profile::_update_fullname()
    {
      {
        auto fullname = this->_fullname->text().trimmed();
        this->_fullname->setText(fullname);
      }
      auto fullname = this->_fullname->text();
      if (fullname != this->_state.me().fullname())
      {
        new FireAndForget(
          [this, fullname]
          {
            if (gap_set_self_fullname(this->_state.state(), QString_to_utf8_string(fullname)) == gap_ok)
              this->_state.me().fullname(fullname);
          }, this);
      }
    }

    void
    Profile::_update_handle()
    {
      {
        auto handle = this->_handle->text().trimmed();
        this->_handle->setText(handle);
      }
      auto handle = this->_handle->text();
      if (handle != this->_state.me().handle())
      {
        new FireAndForget(
          [this, handle]
          {
            if (gap_set_self_handle(this->_state.state(), QString_to_utf8_string(handle)) == gap_ok)
              this->_state.me().handle(handle);
          }, this);
      }
    }

    void
    Profile::_change_avatar()
    {
      QString imagePath = QFileDialog::getOpenFileName(
        this,
        tr("Choose an avatar"),
        "",
        tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" )
        );
      if (!imagePath.isEmpty())
      {
        QPixmap avatar(imagePath);
        avatar = avatar.scaled(512, 512, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->_state.update_avatar(avatar);
      }
    }

    ChangePassword::ChangePassword(QWidget* parent)
      : QDialog(parent)
      , _message(new QLabel(this))
      , _current_password(line_edit("", this))
      , _password(line_edit("", this))
      , _validate(make_button("Validate", this))
    {
      this->_current_password->setEchoMode(QLineEdit::Password);
      this->_password->setEchoMode(QLineEdit::Password);
      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::Base, Qt::white);
      }
      this->setPalette(palette);

      QGridLayout* layout = new QGridLayout(this);
      layout->setContentsMargins(25, 25, 25, 25);
      layout->setSpacing(15);
      {
        auto* warning = section("Changing your password will kick you and reset all your pending transactions!\nAre your sure ?", this);
        warning->setWordWrap(true);
        layout->addWidget(warning, 0, 0, 1, -1);
      }
      layout->addWidget(this->_message, 1, 0, 1, -1, Qt::AlignCenter);
      layout->addWidget(section("Current password", this), 2, 0);
      layout->addWidget(this->_current_password, 2, 1);

      layout->addWidget(section("New assword", this), 3, 0);
      layout->addWidget(this->_password, 3, 1);

      layout->addWidget(this->_validate, 4, 1);
      layout->setRowStretch(5, 1);
      connect(this->_validate, SIGNAL(released()),
              this->_message, SLOT(clear()));
    }

    void
    Profile::_change_password()
    {
      this->_password_window.reset(new ChangePassword(this));
      this->_password_window->setModal(true);
      this->_password_window->show();
      connect(this->_password_window->validate(), SIGNAL(released()),
              this, SLOT(_change_password2()));
      connect(this->_password_window->password(), SIGNAL(returnPressed()),
              this, SLOT(_change_password2()));
    }

    void
    Profile::_change_password2()
    {
      if (this->_password_window->current_password()->text().isEmpty())
      {
        this->_password_window->current_password()->setFocus();
        this->_password_window->message()->setText("You must provide your password");
        return;
      }

      if (this->_password_window->password()->text().isEmpty())
      {
        this->_password_window->password()->setFocus();
        this->_password_window->message()->setText("You must provide a new password");
        return;
      }

      auto res = gap_change_password(
        this->_state.state(),
        QString_to_utf8_string(this->_password_window->current_password()->text()),
        QString_to_utf8_string(this->_password_window->password()->text()));

      if (res == gap_ok)
      {
        this->_password_window->hide();
      }
      else
      {
        auto set_message = [&] (QString const& message)
        {
          this->_password_window->message()->setText(message);
        };

        switch(res)
        {
          case gap_email_password_dont_match:
            set_message("Wrong password.");
            break;
          case gap_password_not_valid:
            set_message("Your password must be at least 3 characters.");
            break;
          default:
            set_message(QString("unknown error %1").arg(res));
        }
       }
    }
  }
}
