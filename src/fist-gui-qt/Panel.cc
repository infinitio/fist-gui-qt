#include <QSizePolicy>
#include <QChildEvent>

// #include <elle/log.hh>

#include <fist-gui-qt/Panel.hh>
#include <fist-gui-qt/globals.hh>

// ELLE_LOG_COMPONENT("infinit.FIST.Panel");

Panel::Panel(Footer* footer,
             QWidget* owner)
  : Super(owner)
  , _body(new SmoothLayout(nullptr, 100))
  , _footer(footer)
{
  // Set parent afterward (to prevent childEvent with body and footer null).
  this->_body->setParent(this);
  this->_footer->setParent(this);

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
Panel::showEvent(QShowEvent* event)
{
  emit set_background_color(Qt::white);
  emit shown();
  Super::showEvent(event);
}

void
Panel::hideEvent(QHideEvent* event)
{
  emit set_background_color(view::background);
  Super::hideEvent(event);
}

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
