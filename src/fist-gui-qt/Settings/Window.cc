#include <iostream>

#include <QBuffer>
#include <QDialog>
#include <QTabWidget>
#include <QFileDialog>
#include <QCoreApplication>
#include <QSpacerItem>
#include <QEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPainter>
#include <QStackedLayout>

#include <elle/container/map.hh>

#include <fist-gui-qt/AvatarIcon.hh>
#include <fist-gui-qt/Settings/Window.hh>
#include <fist-gui-qt/Settings/ui.hh>
#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/State.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("fist.settings.Window");

namespace fist
{
  namespace prefs
  {
    class FireAndForget
      : public QThread
    {
    public:
      typedef std::function<void ()> Action;
    public:
      FireAndForget(Action const& action,
                    QObject* parent)
        : QThread(parent)
        , _action(action)
      {
        connect(this, SIGNAL(finished()), SLOT(deleteLater()));
        this->start();
      }

    private:
      void
      run() override
      {
        this->_action();
      }
    private:
      Action _action;
    };

    class Button
      : public QPushButton
    {
    public:
      Button(QString const& text,
             QWidget* parent)
        : QPushButton(text, parent)
      {
      }

    private:
      void
      enterEvent(QEvent * event) override
      {
        this->setCursor(QCursor(Qt::PointingHandCursor));
        QPushButton::enterEvent(event);
      }

      void
      leaveEvent(QEvent* event) override
      {
        this->setCursor(QCursor(Qt::ArrowCursor));
        QPushButton::leaveEvent(event);
      }
    };

    auto section = [] (QString name,
                       QWidget* parent) -> QLabel*
    {
      auto* section = new QLabel(name, parent);
      view::section::style(*section);
      return section;
    };

    auto make_button = [] (QString name,
                           QWidget* parent) -> QPushButton*
    {
      auto* button = new Button(name, parent);
      button->setStyleSheet(view::button::stylesheet);
      button->setFocusPolicy(Qt::NoFocus);
      return button;
    };

    auto line_edit = [] (QString current,
                         QWidget* parent) -> QLineEdit*
    {
      auto* edit = new QLineEdit(current, parent);
      view::line_edit::style(*edit);
      edit->setEnabled(true);
      return edit;
    };

    auto link = [] (QString text,
                    QWidget* parent) -> QLabel*
    {
      auto* link = new QLabel(text, parent);
      link->setTextInteractionFlags(::view::links::interration_flags);
      link->setOpenExternalLinks(true);
      return link;
    };

    General::General(fist::State& state,
                     QWidget *parent)
      : QWidget(parent)
      , _state(state)
      , _download_folder(new QLabel(this->_state.download_folder(), this))
      , _launch_at_startup(new QCheckBox(this))
      , _device_name(line_edit(this->_state.device().name(), this))
    {
      this->setFocusPolicy(Qt::NoFocus);
      this->_launch_at_startup->setFocusPolicy(Qt::NoFocus);
      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::Base, Qt::white);
      }
      this->setPalette(palette);
      {
        connect(this->_device_name, SIGNAL(editingFinished()),
                this, SLOT(_update_device_name()));
      }
      {
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        this->_launch_at_startup->setCheckState(
          settings.contains("infinit") ? Qt::Checked : Qt::Unchecked);
        connect(this->_launch_at_startup, SIGNAL(stateChanged(int)),
                this, SLOT(_modify_startup_option(int)));
      }
      auto* change_download_folder = make_button("Change", this);
      {
        connect(change_download_folder, SIGNAL(released()),
                this, SLOT(_choose_download_folder()));
      }
      QGridLayout* layout = new QGridLayout(this);
      layout->setContentsMargins(45, 45, 45, 45);
      layout->setSpacing(25);
      layout->setColumnStretch(4, 1);
      layout->addItem(new QSpacerItem(45, 0, QSizePolicy::Fixed), 0, 1, -1);
      {
        auto* general = new QLabel("General", this);
        view::title::style(*general);
        layout->addWidget(general, 0, 0); // , 1, 0);
      }
      layout->addWidget(section("Launch at startup", this), 1, 0);
      layout->addWidget(this->_launch_at_startup, 1, 2);
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->setSpacing(5);
        vlayout->addWidget(section("Download folder", this));
        this->_download_folder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        vlayout->addWidget(this->_download_folder);
        layout->addLayout(vlayout, 2, 0, 1, 2);
      }
      layout->addWidget(change_download_folder, 2, 2, 1, -1, Qt::AlignTop);
      layout->addWidget(section("Device name", this), 3, 0, 1, -1);
      layout->addWidget(this->_device_name, 3, 2, 1, -1);
      layout->addWidget(
        link(view::account::text.arg(this->_state.session_id()), this));
      layout->setRowStretch(5, 1);
    }

    void
    General::showEvent(QShowEvent* event)
    {
      this->_set_download_folder(this->_state.download_folder());
    }

    void
    General::_set_download_folder(QString const& text)
    {
      QFontMetrics metrics(this->_download_folder->font());
      this->updateGeometry();
      this->repaint();
      this->_download_folder->updateGeometry();
      QString elidedText = metrics.elidedText(text, Qt::ElideRight, this->_download_folder->width());
      this->_download_folder->setText(elidedText);
      this->_download_folder->setToolTip(text);
    }

    void
    General::_update_device_name()
    {
      {
        auto device_name = this->_device_name->text().trimmed();
        this->_device_name->setText(device_name);
      }
      auto device_name = this->_device_name->text();
      if (device_name != this->_state.device().name())
      {
        new FireAndForget(
          [this, device_name]
          {
            if (gap_set_device_name(this->_state.state(), QString_to_utf8_string(device_name)) == gap_ok)
              this->_state.device().name(device_name);
          }, this);
      }
    }

    void
    General::_modify_startup_option(int)
    {
#ifdef INFINIT_WINDOWS
      QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
      switch (this->_launch_at_startup->checkState())
      {
         case Qt::Unchecked:
           settings.remove("Infinit");
           fist::settings()["StartUp"].remove("infinit");
           this->_launch_at_startup->setCheckState(Qt::Unchecked);
           break;
         case Qt::Checked:
           settings.setValue("Infinit", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
           fist::settings()["StartUp"].set("infinit", "auto");
           this->_launch_at_startup->setCheckState(Qt::Checked);
           break;
         default:
           break;
       }
#endif
    }

    void
    General::_choose_download_folder()
    {
      QString selected = QFileDialog::getExistingDirectory(
        this,
        tr("Select a download folder"));
      if (!selected.isEmpty())
      {
        this->_state.download_folder(selected);
        this->_set_download_folder(this->_state.download_folder());
      }
    }

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
          link(view::manage_email_addresses::text.arg(this->_state.session_id()),
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
        link(view::account::text.arg(this->_state.session_id()), this));
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
        ELLE_TRACE("res %s", res);
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

    TabBar::TabBar(QWidget* parent)
      : QTabBar(parent)
    {
      new QStackedLayout(this);
      this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      this->setFixedHeight(100);
    }

    void
    TabBar::paintEvent(QPaintEvent* event)
    {
      QTabBar::paintEvent(event);
    }


    void
    TabWidget::paintEvent(QPaintEvent* event)
    {
      QPainter painter(this);
      painter.setPen(QColor(242, 94, 90));
      painter.setBrush(QColor(242, 94, 90));
      painter.drawRect(0, 0, this->width(), this->height());
      QTabWidget::paintEvent(event);
    }

    Window::Window(fist::State& state,
                   QWidget* parent)
      : QDialog(parent)
      , _state(state)
      , _tabWidget(new TabWidget(this))
    {
      auto* layout = new QHBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->addWidget(this->_tabWidget);
      this->_tabWidget->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding);
      this->_tabWidget->addTab(new General(this->_state, this), tr("General"));
      this->_tabWidget->addTab(new Profile(this->_state, this), tr("Profile"));
      this->_tabWidget->setStyleSheet(
        "QTabBar {"
        "  background-color: rgb(242, 94, 90);"
        "  width: 999999px;"
        "}"
        "QTabBar::tab {"
        "  border: 0px;"
        "  height: 100px;"
        "  width: 100px;"
        "  background-color: rgb(242, 94, 90);"
        "  padding: 0px;"
        "  margin: 0px;"
        "  color: white;"
        "  font: bold 13px;"
        "}"
        "QTabBar::tab:selected, QTabBar::tab:hover {"
        " background-color: rgb(213, 86, 83);"
        "}"
        "QTabBar::tab:focus {"
        "  border: 0px;"
        "}"
        );
    }

  }
}
