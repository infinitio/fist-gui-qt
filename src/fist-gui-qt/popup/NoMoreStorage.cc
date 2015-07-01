#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QGridLayout>
#include <QPainter>
#include <QHBoxLayout>
#include <QLabel>

#include <elle/log.hh>

#include <fist-gui-qt/popup/NoMoreStorage.hh>
#include <fist-gui-qt/popup/ui.hh>
#include <fist-gui-qt/utils.hh>
#include <fist-gui-qt/State.hh>

ELLE_LOG_COMPONENT("fist.popup.NoMoreStorage");

namespace fist
{
  namespace popup
  {
    UpgradePlan::UpgradePlan(QString const& title,
                             QString const& text,
                             fist::State const& state,
                             QWidget* parent)
      : QMainWindow(parent, Qt::WindowCloseButtonHint | Qt::Dialog)
      , _state(state)
    {
      {
        auto palette = this->palette();
        palette.setColor(QPalette::Window, view::payment::background);
        this->setPalette(palette);
        this->setAutoFillBackground(true);
      }
      this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
      auto* body = new QWidget(this);
      auto* layout = new QGridLayout(body);
      layout->setContentsMargins(45, 45, 45, 20);
      layout->setVerticalSpacing(30);
      {
        auto* hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 10);
        {
          QLabel* thermometer = new QLabel(this);
          thermometer->setPixmap(
            QPixmap(QString(":/monetisation/thermometer")));
          hlayout->addWidget(thermometer);
        }
        {
          hlayout->addSpacing(35);
        }
        {
          auto* label = new QLabel(this);
          label->setText(title);
          label->setWordWrap(true);
          view::payment::title::style(*label);
          hlayout->addWidget(label, 1);
        }
        layout->addLayout(hlayout, 0, 0, 1, -1);
      }
      {
        auto* label = new QLabel(this);
        label->setText(text);
        label->setTextInteractionFlags(::view::links::interration_flags);
        label->setOpenExternalLinks(true);
        label->setWordWrap(true);
        view::payment::link::style(*label);
        layout->addWidget(label, 1, 0, 1, -1);
      }
      {
        auto* label = new QLabel(this);
        label->setText(view::payment::contact::text);
        label->setWordWrap(true);
        view::payment::contact::style(*label);
        layout->addWidget(label, 2, 0, 1, -1);
      }
      layout->addItem(new QSpacerItem(10, 0, QSizePolicy::Fixed), 3, 0, -1, 1);
      {
        auto* cancel = new QPushButton(
          view::payment::button::cancel::text, this);
        cancel->setCursor(QCursor(Qt::PointingHandCursor));
        cancel->setStyleSheet(view::payment::button::stylesheet.arg(
                                "rgb(158, 158, 158)", "rgb(133, 133, 133)"));
        connect(cancel, SIGNAL(released()), this, SLOT(hide()));
        auto* hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 0);
        hlayout->addWidget(cancel);
        hlayout->addStretch();
        layout->addLayout(hlayout, 4, 0);
      }
      {
        auto* upgrade = new QPushButton(
          view::payment::button::upgrade::text, this);
        upgrade->setCursor(QCursor(Qt::PointingHandCursor));
        upgrade->setStyleSheet(view::payment::button::stylesheet.arg(
                                 "rgb(242, 94, 90)", "rgb(231, 85, 81)"));
        connect(upgrade, SIGNAL(released()),
                this, SLOT(_go_to_website()));
        auto* hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 0);
        hlayout->addStretch();
        hlayout->addWidget(upgrade);
        layout->addLayout(hlayout, 4, 1);
        this->_upgrade = upgrade;
      }
      this->setCentralWidget(body);
      this->setFixedWidth(550);
      this->updateGeometry();
    }

    void
    UpgradePlan::_go_to_website()
    {
      ELLE_TRACE_SCOPE("%s: go to %s", *this, view::payment::link::url);
      QDesktopServices::openUrl(QUrl(view::payment::link::url));
      this->hide();
    }

    void
    UpgradePlan::showEvent(QShowEvent* event)
    {
      this->move(QApplication::desktop()->screen()->rect().center() -
        this->rect().center());
      Super::showEvent(event);
    }

    void
    UpgradePlan::paintEvent(QPaintEvent* event)
    {
      QPainter painter(this);
      auto color = QColor(237, 237, 237);
      painter.setBrush(color);
      painter.setPen(color);
      auto y = this->_upgrade->pos().y();
      auto h = this->_upgrade->height();
      auto pading = this->height() - y - h;
      auto ry = y - pading;
      painter.drawRect(0, ry, this->width(), this->height() - ry);

      auto darker = color.darker(125);
      painter.setBrush(darker);
      painter.setPen(darker);
      painter.drawRect(0, ry, this->width(), 1);
    }

    NoMoreStorage::NoMoreStorage(fist::State& state,
                                 QWidget* parent)
      : UpgradePlan(view::payment::storage::text.arg(
                      readable_size(state.account().link_size_quota.value())),
                    view::payment::storage::body::text,
                    state,
                    parent)
    {
    }

    QString const&
    NoMoreStorage::_campaign() const
    {
      static QString campaign("quota_link");
      return campaign;
    }

    TooBig::TooBig(fist::State& state,
                   QWidget* parent)
      : UpgradePlan(view::payment::too_big::text,
                    view::payment::too_big::body::text,
                    state,
                    parent)
    {
    }

    QString const&
    TooBig::_campaign() const
    {
      static QString campaign("quota_send");
      return campaign;
    }
  };
}
