#ifndef FIST_GUI_QT_SETTINGS_UTILS_HH
# define FIST_GUI_QT_SETTINGS_UTILS_HH

# include <QLineEdit>
# include <QLabel>
# include <QPushButton>
# include <QEvent>
# include <QThread>

#include <fist-gui-qt/Settings/ui.hh>
#include <fist-gui-qt/utils.hh>

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

    QLabel*
    section(QString name, QWidget* parent);

    QPushButton*
    make_button(QString name, QWidget* parent);

    QLineEdit*
    line_edit(QString current, QWidget* parent);

    QLabel*
    link(QString text, QWidget* parent);
  }
}

#endif
