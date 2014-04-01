#include <QSizePolicy>
#include <QChildEvent>

#include <elle/log.hh>

#include <fist-gui-qt/Panel.hh>

// ELLE_LOG_COMPONENT("infinit.FIST.Panel");

Panel::Panel(Footer* footer,
             QWidget* owner):
  SmoothLayout(owner),
  _footer(footer)
{
  this->setHeightHint(this->_footer->size().height());

  this->setMaximumHeight(400);
  this->setFixedWidth(320);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

// /*-------.
// | Layout |
// `-------*/

QSize
Panel::minimumSizeHint() const
{
  return QSize(Super::sizeHint().width(), this->_footer->sizeHint().height());
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

/*----------.
| Printable |
`----------*/
void
Panel::print(std::ostream& stream) const
{
  stream << "Panel";
}
