#include <iostream>

#include "Tabs.hh"

Tabber::Tabber(QWidget* parent)
  : Super(parent)
  , _layout(new QHBoxLayout(this))
  , _active_tab(nullptr)
{
  this->setContentsMargins(0, 0, 0, 0);
  this->_layout->setSpacing(0);
  this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
}

void
Tabber::add_tab(QString const& name,
                QVector<QWidget*> const& widgets)
{
  std::cerr << "add " << name.toStdString() << std::endl;
  this->_tabs.emplace_back(new Tab(*this, name, widgets));
  this->_layout->addWidget(this->_tabs.back().get());

  if (this->_tabs.size() > 1)
    this->_tabs.back()->disable();
  else
    this->active_tab(*this->_tabs.back());
}

void
Tabber::add_tab(QString const& name,
                QWidget* widget)
{
  QVector<QWidget*> widgets;
  widgets.push_back(widget);
  this->add_tab(name, widgets);
}

bool
Tabber::is_active_tab(Tab& tab) const
{
  return &tab == this->_active_tab;
}

void
Tabber::active_tab(Tab& tab)
{
  if (this->is_active_tab(tab))
    return;

  for (auto& _tab: this->_tabs)
  {
    if (_tab.get() != &tab)
    {
      std::cerr << "DISABLE" << std::endl;
      _tab->disable();
    }
  }

  tab.enable();
  this->_active_tab = &tab;
  this->update();
}

void
Tabber::remove_tab(QString const& name)
{
  // this->_tabs.emplace_back(*this, name, widgets);
  // this->_layout->addWidget(this->_tabs.back().widget());
}
