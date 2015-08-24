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
                             QWidget* parent,
                             QString const& cancel_button_text,
                             bool show_invite)
      : QMainWindow(parent, Qt::WindowCloseButtonHint | Qt::Dialog)
      , _state(state)
    {
      {
        auto palette = this->palette();
        palette.setColor(QPalette::Window, view::payment::background);
        this->setPalette(palette);
        this->setAutoFillBackground(true);
      }
      this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
      auto* body = new QWidget(this);
      auto* layout = new QGridLayout(body);
      layout->setContentsMargins(45, 45, 45, 20);
      layout->setVerticalSpacing(30);
      {
        auto* hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 10);
        // hlayout->setSizeConstraint(QLayout::SetMinimumSize);
        {
          QLabel* thermometer = new QLabel(this);
          QVBoxLayout* layout = new QVBoxLayout;
          layout->setContentsMargins(0, 10, 0, 10);
          thermometer->setPixmap(
            QPixmap(QString(":/monetisation/thermometer")));
          layout->addWidget(thermometer);
          hlayout->addLayout(layout);
        }
        {
          hlayout->addSpacing(35);
        }
        {
          auto* label = new QLabel(title, this);
          label->setWordWrap(true);
          view::payment::title::style(*label);
          label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
          hlayout->addWidget(label, 1);
        }
        layout->addLayout(hlayout, 0, 0, 1, -1);
      }
      {
        auto* label = new QLabel(this);
        label->setText(text);
        label->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
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
          cancel_button_text.isEmpty()
          ? view::payment::button::cancel::text
          : cancel_button_text, this);
        cancel->setCursor(QCursor(Qt::PointingHandCursor));
        cancel->setStyleSheet(view::payment::button::stylesheet.arg(
                                "rgb(158, 158, 158)", "rgb(133, 133, 133)"));
        connect(cancel, SIGNAL(released()), this, SLOT(hide()));
        layout->addWidget(cancel, 4, 0, Qt::AlignLeft);
      }
      {
        int index = 0;
        if (show_invite)
        {
          auto* invite = new QPushButton(
            view::payment::button::invite::text, this);
          invite->setCursor(QCursor(Qt::PointingHandCursor));
          invite->setStyleSheet(view::payment::button::stylesheet.arg(
                                  "rgb(242, 94, 90)", "rgb(231, 85, 81)"));
          connect(invite, SIGNAL(released()),
                  this, SLOT(_go_to_referral_page()));
          layout->addWidget(invite, 4, ++index, Qt::AlignRight);
        }

        auto* upgrade = new QPushButton(
          view::payment::button::upgrade::text, this);
        upgrade->setCursor(QCursor(Qt::PointingHandCursor));
        upgrade->setStyleSheet(view::payment::button::stylesheet.arg(
                                 "rgb(242, 94, 90)", "rgb(231, 85, 81)"));
        connect(upgrade, SIGNAL(released()),
                this, SLOT(_go_to_website()));

        layout->addWidget(upgrade, 4, ++index, Qt::AlignRight);
        this->_upgrade = upgrade;
      }
      this->setCentralWidget(body);
      this->setFixedWidth(550);
      this->updateGeometry();
    }

    void
    UpgradePlan::_go_to_website()
    {
      ELLE_TRACE_SCOPE("%s: go to %s", *this, view::payment::storage::body::url);
      this->_state.go_to_online_profile(this->_campaign());
      this->hide();
    }

    void
    UpgradePlan::_go_to_referral_page()
    {
      ELLE_TRACE_SCOPE("%s: go to %s", *this, view::payment::storage::body::url);
      this->_state.go_to_online_profile(this->_campaign(), "/referral");
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
      : UpgradePlan(
        view::payment::storage::text.arg(
          readable_size(state.account().quotas.value().links.quota.get())),
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

    SendToSelfQuotaExceeded::SendToSelfQuotaExceeded(fist::State& state,
                                                     QWidget* parent)
      : UpgradePlan(view::payment::send_to_self::text,
                    view::payment::send_to_self::body::text,
                    state,
                    parent)
    {
    }

    QString const&
    SendToSelfQuotaExceeded::_campaign() const
    {
      static QString campaign("quota_send_to_self");
      return campaign;
    }

    GhostDownloadsLimit::GhostDownloadsLimit(fist::State& state,
                                             QString const& recipient,
                                             QWidget* parent)
      : UpgradePlan(view::payment::ghost_downloads::text.arg(recipient),
                    view::payment::ghost_downloads::body::text,
                    state,
                    parent,
                    "CLOSE",
                    false)
    {
    }

    QString const&
    GhostDownloadsLimit::_campaign() const
    {
      static QString campaign("quota_ghost_downloads_limit");
      return campaign;
    }
  };
}
