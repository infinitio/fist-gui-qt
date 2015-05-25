#include <QPainter>

// #include <elle/log.hh>

#include "Tab.hh"

#include <fist-gui-qt/gui/TabWidget.hh>
#include <fist-gui-qt/globals.hh>

// ELLE_LOG_COMPONENT("fist.gui.Tab");

namespace fist
{
  namespace gui
  {
    Tab::Tab(TabWidget& tabber,
             QString const& name,
             QVector<QWidget*> const& widgets)
      : Super(&tabber)
      , _tabber(tabber)
      , _name(new QLabel(name, this))
      , _notification_count(0)
      , _counter(new QLabel(QString("%1").arg(this->_notification_count), this))
      , _widgets(widgets)
      , _color(0x33, 0x33, 0x33)
    {
      this->setContentsMargins(0, 5, 0, 7);
      auto* layout = new QHBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->addStretch();
      layout->addWidget(this->_name);
      view::tab::counter::style(*this->_counter);
      layout->addWidget(this->_counter);
      layout->addStretch();
      this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

      this->on_notification_count_changed(0);
    }

    void
    Tab::enterEvent(QEvent* event)
    {
      if (!this->_tabber.is_active_tab(*this))
        this->_hover();
      this->setCursor(QCursor(Qt::PointingHandCursor));
      Super::enterEvent(event);
    }

    void
    Tab::leaveEvent(QEvent* event)
    {
      if (this->_tabber.is_active_tab(*this))
        this->_active();
      else
        this->_inactive();
      this->setCursor(QCursor(Qt::ArrowCursor));
      Super::leaveEvent(event);
    }

    void
    Tab::mouseReleaseEvent(QMouseEvent* event)
    {
      this->click();
    }

    void
    Tab::click()
    {
      this->_notify_tabber();
    }

    QSize
    Tab::sizeHint() const
    {
      if (this->_tabber.tabs().size() == 0)
        return QSize(0, 36);

      return QSize(this->_tabber.width() / this->_tabber.tabs().size(),
                   36);
    }

    void
    Tab::on_notification_count_changed(size_t count)
    {
      this->_notification_count = count;
      if (this->_notification_count == 0)
      {
        this->_counter->hide();
      }
      else
      {
        this->_counter->setText(QString("%1").arg(this->_notification_count));
        this->_counter->show();
      }
      this->update();
    }

    QSize
    Tab::minimumSizeHint() const
    {
      return this->sizeHint();
    }

    void
    Tab::paintEvent(QPaintEvent* event)
    {
      int line_height = 2;
      Super::paintEvent(event);
      QPainter painter(this);
      painter.setPen(this->_color);
      painter.setBrush(this->_color);
      painter.drawRect(0, this->height() - line_height, this->width(), line_height);
    }

    void
    Tab::color(QColor const& color)
    {
      this->_color = color;
      this->repaint();
    }

    void
    Tab::_hover()
    {
      if (this->_tabber.is_active_tab(*this))
        return;

      view::tab::hover_style(*this->_name);
      view::tab::counter::hover_style(*this->_counter);
      this->color(view::tab::bar_hover_color);

    }

    void
    Tab::_active()
    {
      view::tab::selected_style(*this->_name);
      view::tab::counter::selected_style(*this->_counter);
      this->color(view::tab::selected_style.color());
    }

    void
    Tab::_inactive()
    {
      view::tab::style(*this->_name);
      view::tab::counter::style(*this->_counter);
      this->color(view::tab::bar_color);
    }

    void
    Tab::_notify_tabber()
    {
      this->_tabber.activate_tab(*this);
    }

    void
    Tab::enable()
    {
      for (QWidget* widget: this->_widgets)
        widget->show();
      this->_active();
      this->update();
      emit activated();
    }

    void
    Tab::disable()
    {
      for (QWidget* widget: this->_widgets)
        widget->hide();
      this->_inactive();
      this->update();
    }

    void
    Tab::print(std::ostream& stream) const
    {
      stream << "Tab(" << this->_name->text().toStdString() << ")";
    }
  }
}
