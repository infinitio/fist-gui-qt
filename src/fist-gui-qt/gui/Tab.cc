#include <iostream>

# include <QPainter>
#include "Tab.hh"

# include <fist-gui-qt/gui/TabWidget.hh>

namespace fist
{
  namespace gui
  {
    Tab::Tab(TabWidget& tabber,
             QString const& name,
             QVector<QWidget*> const& widgets)
      : Super(&tabber)
      , _tabber(tabber)
      , _name(name)
      , _widgets(widgets)
      , _font("Arial", 9)
      , _color(0x33, 0x33, 0x33)
    {
      this->_font.setCapitalization(QFont::AllUppercase);
      this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    }

    void
    Tab::enterEvent(QEvent* event)
    {
      if (!this->_tabber.is_active_tab(*this))
        this->_hover();
      Super::enterEvent(event);
    }

    void
    Tab::leaveEvent(QEvent* event)
    {
      if (this->_tabber.is_active_tab(*this))
        this->_active();
      else
        this->_inactive();
      Super::leaveEvent(event);
    }

    void
    Tab::mouseReleaseEvent(QMouseEvent* event)
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

    QSize
    Tab::minimumSizeHint() const
    {
      return this->sizeHint();
    }

    void
    Tab::paintEvent(QPaintEvent* event)
    {
      int line_height = 2;
      QPainter painter(this);
      painter.setFont(this->_font);
      painter.setPen(this->_color);
      painter.setBrush(this->_color);
      painter.drawText(
        QRect(0, 0, this->width(), this->height() - line_height),
       Qt::AlignCenter, this->name());
      painter.drawRect(0, this->height() - line_height, this->width(), line_height);
    }

    void
    Tab::color(QColor const& color)
    {
      this->_color = color;
      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::WindowText, this->_color);
      }
      this->setPalette(palette);
    }

    void
    Tab::_hover()
    {
      this->color(QColor(0x2B, 0xCE, 0xCD));
    }

    void
    Tab::_active()
    {
      this->color(QColor(0x2B, 0xBE, 0xBD));
    }

    void
    Tab::_inactive()
    {
      this->color(QColor(0x51, 0x51, 0x51));
    }

    void
    Tab::_notify_tabber()
    {
      this->_tabber.active_tab(*this);
    }

    void
    Tab::enable()
    {
      for (QWidget* widget: this->_widgets)
//        if (widget->isHidden())
          widget->show();
      this->_active();
      this->update();
    }

    void
    Tab::disable()
    {
      for (QWidget* widget: this->_widgets)
//        if (widget->isVisible())
          widget->hide();
      this->_inactive();
      this->update();
    }
  }
}
