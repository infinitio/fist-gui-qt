#include <QSizePolicy>
#include <QChildEvent>

#include <elle/log.hh>

#include <fist-gui-qt/Panel.hh>

ELLE_LOG_COMPONENT("infinit.FIST.Panel");

Panel::Panel(Footer* footer,
             QWidget* owner)
  : Super(owner)
  , _body(new SmoothLayout(nullptr, 100))
  , _footer(footer)
{
  ELLE_DEBUG("bite")
    this->_body->setParent(this);
  ELLE_DEBUG("boite")
    this->_footer->setParent(this);
  ELLE_DEBUG("lul");

  this->setHeightHint(this->_footer->size().height());

  this->setMaximumHeight(400);
  this->setFixedWidth(320);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

void
Panel::childEvent(QChildEvent* event)
{
  if (event->child() != this->_footer && event->child() != this->_body)
  {
    if (event->added())
    {
      event->child()->setParent(this->_body);
    }
  }
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
