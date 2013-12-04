#include <QSizePolicy>

#include <fist-gui-qt/Panel.hh>

Panel::Panel(Footer* footer,
             QWidget* owner):
  SmoothLayout(owner),
  _footer(footer)
{
  this->setMaximumHeight(400);
  this->setFixedWidth(320);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
}

QSize
Panel::minimumSizeHint() const
{
  return QSize(320, 0);
}

void
Panel::on_show()
{}

void
Panel::on_hide()
{}

Footer*
Panel::footer()
{
  // XXX: assert.
  return this->_footer;
}
