#include <QHBoxLayout>

#include <elle/finally.hh>

#include <fist-gui-qt/gui/TabWidget.hh>

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
      this->_tabs.emplace_back(new Tab(*this, name, widgets));
      this->_layout->addWidget(this->_tabs.back().get());

      if (this->_tabs.size() > 1)
      {
        this->_tabs.back()->disable();
      }

      this->active_tab(*this->_tabs.begin()->get());
    }

    void
    TabWidget::add_tab(QString const& name,
                       QWidget* widget)
    {
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
    TabWidget::active_tab(Tab& tab)
    {
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
