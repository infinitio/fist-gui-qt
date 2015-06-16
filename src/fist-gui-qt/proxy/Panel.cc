#include <QGridLayout>
#include <QIntValidator>

#include <fist-gui-qt/proxy/Panel.hh>
#include <fist-gui-qt/proxy/ui.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/State.hh>
#include <fist-gui-qt/Settings.hh>

namespace fist
{
  namespace proxy
  {
    Window::Window(State& state,
                     QWidget* parent)
      : QDialog(parent)
      , _activated(new QCheckBox(this))
      , _state(state)
      , _username(nullptr)
      , _password(nullptr)
      , _host(nullptr)
      , _port(nullptr)
      , _entries()
      , _validate(new QPushButton("Validate", this))
    {
      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::Base, Qt::white);
      }
      this->setPalette(palette);

      QGridLayout* layout = new QGridLayout(this);
      layout->setSpacing(15);
      layout->setContentsMargins(45, 45, 45, 45);
      layout->setColumnStretch(3, 1);
      this->_activated->setEnabled(true);
      layout->addItem(new QSpacerItem(45, 0, QSizePolicy::Fixed), 0, 1, -1);
      layout->setColumnStretch(3, -1);
      auto activated = fist::settings()["proxy"].get("activated", "0").toString();
      this->_activated->setCheckState(
          activated == "1" ? Qt::Checked : Qt::Unchecked);
      auto label = [&] (QString const& name) -> QLabel*
        {
          auto* label = new QLabel(name, this);
          view::section::style(*label);
          return label;
        };
      auto line_edit = [&] (QString const& name) -> QLineEdit*
        {
          auto* line_edit = new QLineEdit(name, this);
          view::line_edit::style(*line_edit);
          this->_entries.push_back(line_edit);
          return line_edit;
        };
      {
        this->_validate->setStyleSheet(view::button::stylesheet);
      }
      this->_username = line_edit(fist::settings()["proxy"].get("username", "").toString());
      this->_password = line_edit(fist::settings()["proxy"].get("password", "").toString());
      {
        this->_password->setEchoMode(QLineEdit::Password);
      }
      this->_host = line_edit(fist::settings()["proxy"].get("host", "").toString());
      this->_port = line_edit(fist::settings()["proxy"].get("port", "").toString());
      {
        QIntValidator *qiv = new QIntValidator(0, 65535, this);
        this->_port->setValidator(qiv);
      }
      layout->addWidget(label("Activate proxy"), 0, 0);
      layout->addWidget(this->_activated, 0, 1, 1, 1, Qt::AlignRight);
      layout->addWidget(new HorizontalSeparator(this), 1, 0, 1, -1);
      {
        QHBoxLayout* alignment_layout = new QHBoxLayout;
        alignment_layout->addWidget(label("Host"));
        alignment_layout->addWidget(this->_host, -1);
        layout->addLayout(alignment_layout, 2, 0);
      }
      {
        QHBoxLayout* alignment_layout = new QHBoxLayout;
        alignment_layout->addWidget(label("Port"));
        alignment_layout->addWidget(this->_port, -1);
        layout->addLayout(alignment_layout, 2, 1, 1, -1);
      }
      {
         QHBoxLayout* alignment_layout = new QHBoxLayout;
          alignment_layout->addWidget(label("Username"));
         alignment_layout->addWidget(this->_username, -1);
         layout->addLayout(alignment_layout, 3, 0, 1, -1);
      }
      {
         QHBoxLayout* alignment_layout = new QHBoxLayout;
         alignment_layout->addWidget(label("Password"));
         alignment_layout->addWidget(this->_password, -1);
         layout->addLayout(alignment_layout, 4, 0, 1, -1);
      }
      layout->addWidget(new HorizontalSeparator(this), 5, 0, 1, -1);
      layout->addWidget(
        new QLabel("N.B: SOCKS proxies are not supported yet.", this),
        6, 0, 1, -1);
      layout->addWidget(new HorizontalSeparator(this), 7, 0, 1, -1);
      layout->addWidget(this->_validate, 8, 0, 1, -1, Qt::AlignRight);

      this->_update();

      connect(this->_validate, SIGNAL(released()), this, SLOT(_apply()));
      connect(this->_activated, SIGNAL(stateChanged(int)),
              this, SLOT(_update(int)));
    }

    void
    Window::_update(int)
    {
      for (auto* entry: this->_entries)
        entry->setEnabled(this->_activated->checkState() == Qt::Checked);
    }

    void
    Window::_apply()
    {
      fist::settings()["proxy"].set("username", this->_username->text());
      fist::settings()["proxy"].set("password", this->_password->text());
      fist::settings()["proxy"].set("host", this->_host->text());
      fist::settings()["proxy"].set("port", this->_port->text());
      fist::settings()["proxy"].set(
        "activated", this->_activated->checkState() == Qt::Checked ? "1" : "0");
      this->hide();
      emit done();
    }
  }
}
