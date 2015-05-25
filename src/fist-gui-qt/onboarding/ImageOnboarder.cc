#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMovie>
#include <QKeyEvent>

#include <fist-gui-qt/onboarding/ImageOnboarder.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/onboarding/ui.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/IconButton.hh>

#include <elle/assert.hh>

namespace fist
{
  namespace onboarding
  {
    Dots::Dots(int count,
             QWidget* parent)
      : _count(count)
      , _current(-1)
      , _diameter(10)
      , _margin(7)
    {
      this->setFixedSize(
        count * (this->_diameter + this->_margin) + this->_margin,
        this->_diameter + this->_margin * 2);
    }

    void
    Dots::count(unsigned int count)
    {
      this->_count = count;
      this->setFixedSize(
        count * (this->_diameter + this->_margin) + this->_margin,
        this->_diameter + this->_margin * 2);
    }

    void
    Dots::paintEvent(QPaintEvent* event)
    {
      QPainter painter(this);
      painter.setPen(Qt::NoPen);
      for (unsigned int i = 0; i < this->_count; ++i)
      {
        painter.setBrush(
          i == this->_current
          ? QColor(0x33, 0x33, 0x33)
          : QColor(0xAE, 0xAE, 0xAE));
        painter.drawEllipse(
          this->_margin + (i * (this->_diameter + this->_margin)),
          this->_margin,
          this->_diameter, this->_diameter);
      }
    }

    Landing::Landing(QWidget* parent)
      : Super(parent)
    {
      QGridLayout* grid = new QGridLayout(this);
      grid->setContentsMargins(25, 25, 25, 25);
      grid->setSpacing(10);
      auto* logo = new QLabel(this);
      logo->setPixmap(QPixmap(":/login/logo"));
      grid->addWidget(logo, 0, 0, 1, 0, Qt::AlignCenter);
      grid->setRowStretch(1, 1);
      auto filler = [this, grid] (QString const& icon,
                                  QString const& title,
                                  QString const& body,
                                  int x,
                                  int y)
      {
        QLabel* _icon = new QLabel(this);
        _icon->setPixmap(QPixmap(icon));
        QLabel* _title = new QLabel(title, this);
        view::landing::title::style(*_title);
        QLabel* _body = new QLabel(body, this);
        view::landing::body::style(*_body);
        _body->setWordWrap(true);
        grid->addWidget(_icon, y, x);
        grid->addWidget(_title, y + 1, x);
        grid->addWidget(_body, y + 2, x);
      };

      filler(view::landing::no_limits::image,
             view::landing::no_limits::title,
             view::landing::no_limits::body,
             0,
             2);
      filler(view::landing::speed::image,
             view::landing::speed::title,
             view::landing::speed::body,
             1,
             2);
      grid->setRowStretch(5, 1);
      filler(view::landing::unstoppable::image,
             view::landing::unstoppable::title,
             view::landing::unstoppable::body,
             0,
             6);
      filler(view::landing::secure::image,
             view::landing::secure::title,
             view::landing::secure::body,
             1,
             6);
      grid->setRowStretch(9, 1);
    }

    Panel::Panel(QString const& title,
                 QString const& icon,
                 QString const& body,
                 QWidget* parent)
      : Super(parent)
      , _title(new QLabel(title, this))
      , icon(new QMovie(icon, QByteArray(), this))
    {
      QVBoxLayout* layout = new QVBoxLayout(this);
      QHBoxLayout* hlayout = new QHBoxLayout;
      this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(5);
      hlayout->setContentsMargins(0, 0, 0, 0);
      hlayout->setSpacing(25);
      view::screen::title::style(*this->_title);
      auto* picture = new QLabel(this);
      this->icon->start();
      picture->setMovie(this->icon);
      auto* description = new QLabel(body);
      view::screen::body::style(*description);
      description->setWordWrap(true);
      description->setMinimumWidth(300);
      layout->addWidget(this->_title, 0, Qt::AlignCenter);
      layout->addSpacing(45);
      {
        hlayout->addWidget(picture);
        hlayout->addWidget(description);
      }
      layout->addLayout(hlayout);
    }

    void
    Panel::paintEvent(QPaintEvent* event)
    {
      QPainter painter(this);
      painter.setPen(Qt::NoPen);
      painter.setBrush(Qt::black);
      painter.drawRect(
        this->_title->x(), this->_title->y() + this->_title->height() + 8,
        this->_title->width(), 1);
      Super::paintEvent(event);
    }

    ImageOnboarder::ImageOnboarder(QWidget* widget)
      : Super(widget)
      , _images()
      , _prev(new QPushButton(tr("Previous"), this))
      , _foo(new Dots(0, this))
      , _next(new QPushButton(tr("Next"), this))
    {
      this->setWindowIcon(QIcon(QPixmap(":/logo")));
      this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
      QWidget* w = new QWidget(this);
      QVBoxLayout* layout = new QVBoxLayout(w);
      layout->setContentsMargins(35, 35, 35, 35);
      layout->setSpacing(0);
      {
        auto* screen = new Landing(this);
        this->_add(screen);
        layout->addWidget(screen);
      }
      auto add_screen = [&] (QString const& title,
                             QString const& icon,
                             QString const& body)
      {
        auto* screen = new Panel(title, icon, body, this);
        this->_add(screen);
        layout->addWidget(screen, 0, Qt::AlignCenter);
      };
      add_screen(view::send_to_email::title,
                 view::send_to_email::image,
                 view::send_to_email::body);
      add_screen(view::send_to_infinit_user::title,
                 view::send_to_infinit_user::image,
                 view::send_to_infinit_user::body);
      add_screen(view::receive::title,
                 view::receive::image,
                 view::receive::body);
      add_screen(view::send_to_self::title,
                 view::send_to_self::image,
                 view::send_to_self::body);
      add_screen(view::always_available::title,
                 view::always_available::image,
                 view::always_available::body);
      layout->addSpacing(20);
      {
        QHBoxLayout* hlayout = new QHBoxLayout;
        hlayout->addStretch();
        connect(this->_next, SIGNAL(released()), this, SLOT(next()));
        this->_next->setStyleSheet(
          QString("QPushButton {"
                  "  background-color: %1;"
                  "  color: white;"
                  "  font: bold 15px;"
                  "  border-radius: 17px;"
                  "  height: 36px;"
                  "  padding-left: 18px;"
                  "  padding-right: 18px;"
                  "}"
                  "QPushButton:focused, QPushButton:hover {"
                  "  color: white;"
                  "  background-color: %2;"
                  "}"
            ).arg("rgb(242, 94, 90)", "rgb(231, 85, 81)"));
        this->_next->setFocusPolicy(Qt::NoFocus);
        connect(this->_prev, SIGNAL(released()), this, SLOT(prev()));
        this->_prev->setStyleSheet(
          QString("QPushButton {"
                  "  background-color: %1;"
                  "  color: white;"
                  "  font: bold 15px;"
                  "  border-radius: 17px;"
                  "  height: 36px;"
                  "  padding-left: 16px;"
                  "  padding-right: 16px;"
                  "}"
                  "QPushButton:focused, QPushButton:hover {"
                  "  background-color: %2;"
                  "}"
                  "QPushButton:disabled {"
                  "  background-color: %3;"
                  "}"
            ).arg("rgb(158, 158, 158)", "rgb(133, 133, 133)", "rgb(198, 198, 198)"));
        this->_prev->setFocusPolicy(Qt::NoFocus);
        hlayout->addWidget(this->_prev, 1, Qt::AlignLeft);
        hlayout->addWidget(this->_foo, 0, Qt::AlignCenter);
        hlayout->addWidget(this->_next, 1, Qt::AlignRight);
        layout->addLayout(hlayout);
      }
      this->setCentralWidget(w);

      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::Base, Qt::white);
      }
      this->setPalette(palette);

      this->setFixedSize(QSize{960, 720});
      this->next();
    }

    void
    ImageOnboarder::_add(QWidget* widget)
    {
      this->_images.emplace_back(widget);
      widget->setParent(this);
      widget->hide();
      this->_foo->count(this->_foo->count() + 1);
    }

    void
    ImageOnboarder::keyPressEvent(QKeyEvent* event)
    {
#ifdef FIST_PRODUCTION_BUILD
      if (event->key() == Qt::Key_Left)
      {
        this->prev();
        event->accept();
      }
      else if (event->key() == Qt::Key_Right)
      {
        this->next();
        event->accept();
      }
      else
#endif
      {
        QWidget::keyPressEvent(event);
      }
    }

    void
    ImageOnboarder::next(int i)
    {
      if (this->_images.size() == 0)
      {
        this->_prev->hide();
        this->_next->hide();
        return;
      }
      if (i == 0)
        i = 1;
      this->_prev->show();
      this->_next->show();
      for (auto* widget: this->_images)
        widget->hide();
      this->_foo->current((this->_foo->current() + i) % this->_foo->count());
      this->_prev->setEnabled(this->_foo->current() != 0);
      disconnect(this->_next, 0, 0, 0);
      if (this->_foo->current() == this->_images.size() - 1)
      {
        this->_next->setText(tr("Get started"));
        connect(this->_next, SIGNAL(released()), this, SLOT(close()));
        connect(this->_next, SIGNAL(released()), this, SIGNAL(onboarded()));
      }
      else
      {
        this->_next->setText(tr("Next"));
        connect(this->_next, SIGNAL(released()), this, SLOT(next()));
      }
      {
        auto* widget = this->_images[this->_foo->current()];
        widget->show();
        if (dynamic_cast<Panel*>(widget) != nullptr)
        {
          static_cast<Panel*>(widget)->icon->jumpToFrame(0);
        }
      }
      this->updateGeometry();
      this->repaint();
    }

    void
    ImageOnboarder::prev()
    {
      this->next(this->_foo->count() - 1);
    }
  }
}
