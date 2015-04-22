#include <QApplication>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <fist-gui-qt/popup/NoMoreStorage.hh>
#include <fist-gui-qt/popup/ui.hh>

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
      auto* body = new QWidget(this);
      auto* layout = new QGridLayout(body);
      layout->setContentsMargins(50, 50, 50, 50);
      layout->setVerticalSpacing(30);
      {
        auto* hlayout = new QHBoxLayout;
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
                                "rgb(157, 157, 157)", "rgb(130, 130, 130)"));
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
        upgrade->setStyleSheet(view::payment::button::stylesheet);
        upgrade->setStyleSheet(view::payment::button::stylesheet.arg(
                                 "rgb(248, 93, 91)", "rgb(255, 95, 94)"));
        auto* hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 0);
        hlayout->addStretch();
        hlayout->addWidget(upgrade);
        layout->addLayout(hlayout, 3, 1);
      }
      this->setCentralWidget(body);
      this->updateGeometry();

      // this->setFixedSize(
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
