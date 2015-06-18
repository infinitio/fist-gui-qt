#include <fist-gui-qt/notification/IncomingTransaction.hh>
# include <fist-gui-qt/notification/ui.hh>

# include <fist-gui-qt/model/Transaction.hh>
# include <fist-gui-qt/model/User.hh>

# include <QVBoxLayout>
# include <QHBoxLayout>
# include <QPainter>
# include <QPushButton>
# include <QLabel>

namespace fist
{
  namespace notification
  {
    static float icon_diameter = 22;

    IncomingTransaction::IncomingTransaction(
      model::Transaction const& transaction,
      QWidget* parent)
      : Super(10000, nullptr)
      , _transaction(transaction)
    {
      auto* layout = new QHBoxLayout(this);
      layout->setContentsMargins(view::spacing, 0, 0, 0);
      layout->setSpacing(view::spacing);
      {
        QVBoxLayout* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, view::spacing, 0, view::spacing + icon_diameter * 0.33);
        vlayout->addWidget(this->_icon, 0, Qt::AlignTop);
        layout->addLayout(vlayout);
      }
      layout->addSpacing(view::spacing);
      {
        QVBoxLayout* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, view::spacing, 0, 0);
        auto* label = new QLabel(
          incoming_transaction::view::body::text
          .arg(transaction.peer().fullname())
          .arg((transaction.files().size() == 1)
               ? transaction.files()[0]
               : QString("%1 files").arg(transaction.files().size())),
          this);
        view::body::style(*label);
        label->setWordWrap(true);
        vlayout->addWidget(label, 1, Qt::AlignTop);
        vlayout->addStretch();
        layout->addLayout(vlayout, 1);
      }
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->setSpacing(0);
        {
          QPushButton* accept = new QPushButton(QString::fromUtf8("✓"), this);
          accept->setFocusPolicy(Qt::NoFocus);
          accept->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
          accept->setStyleSheet("QPushButton {"
                                 "  background-color: rgb(190, 239, 102);"
                                 "  border-radius: 0px;"
                                 "  color: white;"
                                 "  padding-top: 5px;"
                                 "  padding-bottom: 5px;"
                                 "  padding-left: 15px;"
                                 "  padding-right: 15px;"
                                 "  font: bold 18px;"
                                 "}");
          vlayout->addWidget(accept);
          connect(accept, SIGNAL(released()), this, SLOT(_accept()));
          connect(accept, SIGNAL(released()), this, SLOT(hide()));
        }
        {
          QPushButton* later = new QPushButton("Later", this);
          later->setFocusPolicy(Qt::NoFocus);
          later->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
          later->setStyleSheet("QPushButton {"
                                 "  background-color: rgb(215, 215, 215);"
                                 "  border-radius: 0px;"
                                 "  color: white;"
                                 "  padding-top: 5px;"
                                 "  padding-bottom: 5px;"
                                 "  padding-left: 15px;"
                                 "  padding-right: 15px;"
                                 "  font: 14px;"
                                 "}");
          vlayout->addWidget(later);
          connect(later, SIGNAL(released()), this, SLOT(hide()));
        }
        layout->addLayout(vlayout);
      }
      this->resize(this->minimumSizeHint());
    }

    void
    IncomingTransaction::_accept()
    {
      emit accept_transaction(this->_transaction.id());
    }

    void
    IncomingTransaction::paintEvent(QPaintEvent* event)
    {
      int margin = 0;
      if (this->_avatar.isNull())
      {
        QPixmap mask(view::icon::diameter, view::icon::diameter);
        {
          mask.fill(Qt::transparent);
          QPainter painter(&mask);
          painter.setRenderHint(QPainter::Antialiasing, true);
          painter.setPen(Qt::NoPen);
          painter.setBrush(Qt::black);
          painter.drawEllipse(0, 0, view::icon::diameter, view::icon::diameter);
        }

        this->_avatar = QPixmap(view::icon::diameter + 2 * margin, view::icon::diameter + 2 * margin + icon_diameter / 2);
        this->_avatar.fill(Qt::transparent);
        QPainter paint(&this->_avatar);
        paint.setRenderHint(QPainter::Antialiasing);
        paint.setPen(view::background);
        paint.setBrush(view::background);
        paint.drawPixmap(
          margin, margin + 5,
          this->_transaction.peer().avatar().scaled(
            view::icon::diameter, view::icon::diameter, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        paint.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        paint.drawPixmap(margin, margin + 5, mask);
        paint.setCompositionMode(QPainter::CompositionMode_SourceOver);
        float factor = 0.10;
        QPixmap logo(icon_diameter * (1 + 2 * factor), icon_diameter * (1 + 2 * factor));
        {
          logo.fill(Qt::transparent);
          QPainter paint(&logo);
          paint.setPen(view::background);
          paint.setBrush(view::background);
          paint.setRenderHint(QPainter::Antialiasing);
          paint.setRenderHint(QPainter::HighQualityAntialiasing);
          paint.drawEllipse(
            0, 0, icon_diameter * (1 + 2 * factor), icon_diameter * (1 + 2 * factor));
          paint.drawPixmap(
            (float) icon_diameter * factor + 1,
            (float) icon_diameter * factor + 1,
            QPixmap(":/notification/logo").scaled(
              icon_diameter, icon_diameter,
              Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        paint.drawPixmap(margin + view::icon::diameter / 2,
                         margin + view::icon::diameter - (float) logo.height() * 0.66 + 5,
                         logo);
        this->_icon->setPixmap(this->_avatar);
        this->_icon->updateGeometry();
      }
      Super::paintEvent(event);
    }
  }
}
