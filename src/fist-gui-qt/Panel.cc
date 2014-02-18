#include <QSizePolicy>
#include <QChildEvent>

#include <fist-gui-qt/Panel.hh>

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

void
Panel::childEvent(QChildEvent* event)
{
  Super::childEvent(event);

  if (event->added() || event->removed())
  {
    if (this->_footer != nullptr)
    {
      // Ensure that the footer is always the last element added.
      this->_footer->setParent(nullptr);
      this->_footer->setParent(this);
    }
  }
}

Footer*
Panel::footer()
{
  // XXX: assert.
  return this->_footer;
}
