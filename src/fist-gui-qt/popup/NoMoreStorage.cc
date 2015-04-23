#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <elle/log.hh>

#include <fist-gui-qt/popup/NoMoreStorage.hh>
#include <fist-gui-qt/popup/ui.hh>

ELLE_LOG_COMPONENT("fist.popup.NoMoreStorage");

namespace fist
{
  namespace popup
  {
    NoMoreStorage::NoMoreStorage(QWidget* parent)
      : QMainWindow(parent, Qt::WindowCloseButtonHint | Qt::Dialog)
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
      layout->setContentsMargins(45, 45, 45, 35);
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
          label->setText(view::payment::main::text);
          label->setWordWrap(true);
          view::payment::main::style(*label);
          hlayout->addWidget(label, 1);
        }
        layout->addLayout(hlayout, 0, 0, 1, -1);
      }
      {
        auto* label = new QLabel(this);
        label->setText(view::payment::link::text);
        label->setTextInteractionFlags(::links::interration_flags);
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
        layout->addLayout(hlayout, 3, 0);
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
        layout->addLayout(hlayout, 3, 1);
      }
      this->setCentralWidget(body);
      this->updateGeometry();
      this->setFixedWidth(550);
    }

    void
    NoMoreStorage::_go_to_website()
    {
      ELLE_TRACE_SCOPE("%s: go to %s", *this, view::payment::link::url);
      QDesktopServices::openUrl(QUrl(view::payment::link::url));
      this->hide();
    }

    void
    NoMoreStorage::showEvent(QShowEvent* event)
    {
      this->move(QApplication::desktop()->screen()->rect().center() -
        this->rect().center());
      Super::showEvent(event);
    }
  };
}
