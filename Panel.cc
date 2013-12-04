#include <QSizePolicy>

#include <fist-gui-qt/Panel.hh>

Panel::Panel(Footer* footer,
             QWidget* owner):
  SmoothLayout(owner),
  _footer(footer)
{
  this->setHeightHint(this->_footer->size().height());
  this->setMaximumHeight(400);
  this->setFixedWidth(320);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
}

QSize
Panel::minimumSizeHint() const
{
  return QSize(320, this->_footer->sizeHint().height());
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
