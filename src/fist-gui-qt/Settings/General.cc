#include <QCoreApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QRegExpValidator>

#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/Settings/General.hh>
#include <fist-gui-qt/Settings/utils.hh>
#include <fist-gui-qt/State.hh>
#include <fist-gui-qt/utils.hh>

namespace fist
{
  namespace prefs
  {
    General::General(fist::State& state,
                     QWidget *parent)
      : QWidget(parent)
      , _state(state)
      , _download_folder(new QLabel(this->_state.download_folder(), this))
#ifdef INFINIT_WINDOWS
      , _launch_at_startup(new QCheckBox(this))
#endif
      , _device_name(line_edit(this->_state.device().name(), this))
    {
      int i = -1;
      this->setFocusPolicy(Qt::NoFocus);
      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::Base, Qt::white);
      }
      this->setPalette(palette);
      {
        connect(this->_device_name, SIGNAL(editingFinished()),
                this, SLOT(_update_device_name()));
        QRegExpValidator* validator =
          new QRegExpValidator(QRegExp(".{1,64}", Qt::CaseInsensitive), this);
        this->_device_name->setValidator(validator);
      }
      QGridLayout* layout = new QGridLayout(this);
      layout->setContentsMargins(45, 45, 45, 45);
      layout->setSpacing(25);
      layout->setColumnStretch(4, 1);
      layout->addItem(new QSpacerItem(45, 0, QSizePolicy::Fixed), 0, 1, -1);
      {
        auto* general = new QLabel("General", this);
        view::title::style(*general);
        layout->addWidget(general, ++i, 0); // , 1, 0);
      }
      auto* change_download_folder = make_button("Change", this);
      {
        connect(change_download_folder, SIGNAL(released()),
                this, SLOT(_choose_download_folder()));
      }
#ifdef INFINIT_WINDOWS
      {
        this->_launch_at_startup->setFocusPolicy(Qt::NoFocus);
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        this->_launch_at_startup->setCheckState(
          settings.contains("infinit") ? Qt::Checked : Qt::Unchecked);
        connect(this->_launch_at_startup, SIGNAL(stateChanged(int)),
                this, SLOT(_modify_startup_option(int)));
      }
      layout->addWidget(section("Launch at startup", this), ++i, 0);
      layout->addWidget(this->_launch_at_startup, i, 2);
#endif
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->setSpacing(5);
        vlayout->addWidget(section("Download folder", this));
        this->_download_folder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        vlayout->addWidget(this->_download_folder);
        layout->addLayout(vlayout, ++i, 0, 1, 2);
      }
      layout->addWidget(change_download_folder, i, 2, 1, -1, Qt::AlignTop);
      layout->addWidget(section("Device name", this), ++i, 0, Qt::AlignTop | Qt::AlignLeft);
      layout->addWidget(this->_device_name, i, 2, 1, -1);
      layout->addWidget(
        link(view::account::text.arg(QString(QUrl::toPercentEncoding(this->_state.session_id()))), this),
        ++i, 0, 1, -1);
      layout->setRowStretch(++i, 1);
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
        if (device_name.isEmpty())
          return;
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
  }
}
