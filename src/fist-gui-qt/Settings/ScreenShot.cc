#include <QGridLayout>

#include <elle/log.hh>

#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/Settings/ScreenShot.hh>
#include <fist-gui-qt/Settings/utils.hh>
#include <fist-gui-qt/State.hh>

ELLE_LOG_COMPONENT("fist.prefs.ScreenShot");

namespace fist
{
  namespace prefs
  {
    ScreenShot::ScreenShot(fist::State& state,
                           QWidget *parent)
      : Super(parent)
      , _state(state)
      , _fullscreen(new fist::gui::EnterKeySequence(
                      fist::settings()["Shortcuts"].get("fullscreen", "Ctrl+Shift+I").toString(), this))
      , _region(new fist::gui::EnterKeySequence(
                  fist::settings()["Shortcuts"].get("region", "Ctrl+Shift+O").toString(), this))
      , _message(new QLabel(this))
    {
      view::line_edit::style(*this->_fullscreen);
      view::line_edit::style(*this->_region);

      this->_fullscreen->installEventFilter(this);
      this->_region->installEventFilter(this);
      connect(this->_fullscreen, SIGNAL(no_modifier()),
              this, SLOT(_no_modifiers()));
      connect(this->_fullscreen, SIGNAL(textChanged(QString const&)),
              this, SLOT(_change_fullscreen_shortcut(QString const&)));
      connect(this->_region, SIGNAL(no_modifier()),
              this, SLOT(_no_modifiers()));
      connect(this->_region, SIGNAL(textChanged(QString const&)),
              this, SLOT(_change_region_shortcut(QString const&)));
      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::Base, Qt::white);
      }
      this->setPalette(palette);

      this->setFont(view::section::style.font());
      this->setFocusPolicy(Qt::NoFocus);
      QGridLayout* layout = new QGridLayout(this);
      layout->setSpacing(25);
      layout->setContentsMargins(45, 45, 45, 45);
      layout->setColumnStretch(3, 1);
      layout->addItem(new QSpacerItem(45, 0, QSizePolicy::Fixed), 0, 1, -1);
      {
        auto* screenshots = new QLabel("Screenshots", this);
        view::title::style(*screenshots);
        layout->addWidget(screenshots, 0, 0); // , 1, 0);
      }

      layout->addWidget(section("Fullscreen", this), 1, 0);
      layout->addWidget(this->_fullscreen, 1, 2);

      layout->addWidget(section("Region", this), 2, 0);
      layout->addWidget(this->_region, 2, 2);
      layout->addWidget(this->_message, 3, 0, 1, -1);
      this->_message->setWordWrap(true);
      layout->setRowStretch(4, 1);
    }

    bool
    ScreenShot::eventFilter(QObject *obj, QEvent *event)
    {
      if (event->type() == QEvent::FocusIn ||
          event->type() == QEvent::FocusOut)
      {
        if (obj == this->_fullscreen)
        {
          ELLE_DEBUG("fullscreen field %s focus", event->type() == QEvent::FocusIn ? "gain" : "lost");
          this->_state.fullscreen_screenshot->setEnabled(event->type() == QEvent::FocusOut);
        }
        else if (obj == this->_region)
        {
          ELLE_DEBUG("region field %s focus", event->type() == QEvent::FocusIn ? "gain" : "lost");
           this->_state.region_screenshot->setEnabled(event->type() == QEvent::FocusOut);
        }
      }
      return QWidget::eventFilter(obj, event);
    }

    void
    ScreenShot::_no_modifiers()
    {
      this->_set_message("You must use a modifier (ALT, CTRL or SHIFT)");
    }

    void
    ScreenShot::_set_message(QString const& message)
    {
      this->_message->setText(message);
    }


    bool
    ScreenShot::_validate_key_sequence(QString const& c,
                                       QKeySequence const& other_sequence)
    {
      QKeySequence sequence(c);
      if (sequence == other_sequence)
      {
        this->_set_message("Sequences must be different");
        return false;
      }
      return true;
    }

    void
    ScreenShot::_change_fullscreen_shortcut(QString const& sequence)
    {
      ELLE_LOG("fullscreen: %s", sequence);
      if (this->_validate_key_sequence(sequence, this->_region->sequence()))
      {
        fist::settings()["Shortcuts"].set("fullscreen", sequence);;
        this->_state.update_fullscreen_screenshot_shortcut();
      }
      else
      {
        this->_fullscreen->rollback();
      }
    }

    void
    ScreenShot::_change_region_shortcut(QString const& sequence)
    {
      ELLE_LOG("region: %s", sequence);
      if (this->_validate_key_sequence(sequence, this->_fullscreen->sequence()))
      {
        fist::settings()["Shortcuts"].set("region", sequence);
        this->_state.update_region_screenshot_shortcut();
      }
      else
      {
        this->_region->rollback();
      }
    }
  }
}
