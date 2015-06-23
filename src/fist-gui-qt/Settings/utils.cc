#include <fist-gui-qt/Settings/utils.hh>

namespace fist
{
  namespace prefs
  {
    QLabel*
    section(QString name, QWidget* parent)
    {
      auto* section = new QLabel(name, parent);
      view::section::style(*section);
      return section;
    }

    QPushButton*
    make_button(QString name, QWidget* parent)
    {
      auto* button = new Button(name, parent);
      button->setStyleSheet(view::button::stylesheet);
      button->setFocusPolicy(Qt::NoFocus);
      return button;
    }

    QLineEdit*
    line_edit(QString current, QWidget* parent)
    {
      auto* edit = new QLineEdit(current, parent);
      view::line_edit::style(*edit);
      edit->setEnabled(true);
      return edit;
    }

    QLabel*
    link(QString text, QWidget* parent)
    {
      auto* link = new QLabel(text, parent);
      link->setTextInteractionFlags(::view::links::interration_flags);
      link->setOpenExternalLinks(true);
      return link;
    }
  }
}
