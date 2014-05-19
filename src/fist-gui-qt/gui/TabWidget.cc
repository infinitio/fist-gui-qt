#include <QEvent>
#include <QKeyEvent>
#include <QHBoxLayout>

#include <elle/finally.hh>
#include <elle/log.hh>

#include <fist-gui-qt/gui/TabWidget.hh>

ELLE_LOG_COMPONENT("infinit.FIST.gui.TabWidget");

namespace fist
{
  namespace gui
  {

    TabWidget::TabWidget(QWidget* parent)
      : Super(parent)
      , _layout(new QHBoxLayout(this))
      , _active_tab(nullptr)
    {
      this->setContentsMargins(0, 0, 0, 0);
      this->_layout->setSpacing(0);
      this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
    }

    void
    TabWidget::add_tab(QString const& name,
                       QVector<QWidget*> const& widgets)
    {
      ELLE_TRACE_SCOPE("%s: add tab %s", *this, name);
      this->_tabs.emplace_back(new Tab(*this, name, widgets));
      this->_layout->addWidget(this->_tabs.back().get());

      if (this->_tabs.size() > 1)
      {
        this->_tabs.back()->disable();
      }

      this->activate_tab(*this->_tabs.begin()->get());
    }

    void
    TabWidget::add_tab(QString const& name,
                       QWidget* widget)
    {
      ELLE_TRACE_SCOPE("%s: add tab %s", *this, name);
      QVector<QWidget*> widgets;
      widgets.push_back(widget);
      this->add_tab(name, widgets);
    }

    bool
    TabWidget::is_active_tab(Tab& tab) const
    {
      return &tab == this->_active_tab;
    }

    void
    TabWidget::activate_tab(Tab& tab)
    {
      ELLE_TRACE_SCOPE("%s: activate tab %s", *this, tab);
      elle::SafeFinally update([&] { tab.enable(); this->update(); });
      if (this->is_active_tab(tab))
      {
        return;
      }

      for (auto& _tab: this->_tabs)
      {
        if (_tab.get() != &tab)
        {
          _tab->disable();
        }
      }

      this->_active_tab = &tab;
    }

    int
    TabWidget::_active_tab_index() const
    {
      // XXX: Dirty.
      int i = 0;
      for (auto const& tab: this->_tabs)
      {
        if (this->is_active_tab(*tab))
          break;
        ++i;
      }

      return i;
    }
    void
    TabWidget::activate_next()
    {
      ELLE_TRACE_SCOPE("%s: activate next tab", *this);
      if (this->_tabs.empty())
        return;

      int current_tab = this->_active_tab_index();
      this->activate_tab(*this->_tabs[++current_tab % this->_tabs.size()]);
    }

    void
    TabWidget::activate_previous()
    {
      ELLE_TRACE_SCOPE("%s: activate previous tab", *this);
      if (this->_tabs.empty())
        return;
      int current_tab = this->_active_tab_index();
      this->activate_tab(*this->_tabs[--current_tab % this->_tabs.size()]);
    }

    void
    TabWidget::activate_first()
    {
      ELLE_TRACE_SCOPE("%s: activate first tab", *this);
      if (this->_tabs.empty())
        return;

      this->activate_tab(**this->_tabs.begin());
    }

    void
    TabWidget::remove_tab(QString const& name)
    {
      elle::unreachable();
    }

    QSize
    TabWidget::sizeHint() const
    {
      return QSize(320, Super::sizeHint().height());
    }

  }
}
