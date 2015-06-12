#ifndef FIST_GUI_QT_SETTINGS_UTILS_HH
# define FIST_GUI_QT_SETTINGS_UTILS_HH

# include <QLineEdit>
# include <QLabel>
# include <QPushButton>
# include <QEvent>
# include <QThread>

#include <fist-gui-qt/Settings/ui.hh>

namespace fist
{
  namespace prefs
  {
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

    static
    QLabel*
    section(QString name,
            QWidget* parent)
    {
      auto* section = new QLabel(name, parent);
      view::section::style(*section);
      return section;
    }

    static
    QPushButton*
    make_button(QString name,
                QWidget* parent)
    {
      auto* button = new Button(name, parent);
      button->setStyleSheet(view::button::stylesheet);
      button->setFocusPolicy(Qt::NoFocus);
      return button;
    }

    static
    QLineEdit*
    line_edit(QString current,
              QWidget* parent)
    {
      auto* edit = new QLineEdit(current, parent);
      view::line_edit::style(*edit);
      edit->setEnabled(true);
      return edit;
    }

    static
    QLabel*
    link(QString text,
         QWidget* parent)
    {
      auto* link = new QLabel(text, parent);
      link->setTextInteractionFlags(::view::links::interration_flags);
      link->setOpenExternalLinks(true);
      return link;
    }

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
  }
}

#endif
