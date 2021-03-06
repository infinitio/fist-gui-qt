#include <QHBoxLayout>

#include <fist-gui-qt/SendView/Message.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/SendView/ui.hh>

 namespace fist
{
  namespace sendview
  {
    Message::Message(QWidget* parent,
                     int maximum_characters)
      : Super(parent)
      , _top_separator(new HorizontalSeparator(this))
      , _icon()
      , _message(new QLineEdit(this))
      , _maximum_characters(maximum_characters)
      , _remaining_characters(QString("%1").arg(maximum_characters))
    {
      this->setContentsMargins(0, 0, 0, 0);
      {
        this->_icon.setPixmap(QPixmap(":/send/note"));
      }
      {
        this->_message->setMaxLength(100);
        view::message::style(*this->_message);
        this->_message->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        this->_message->setFrame(false);
        this->_message->setPlaceholderText("Optional message...");
        this->_message->setFixedHeight(QPixmap(":/send/note").height());
      }
      {
        view::message::remaining_characters::style(this->_remaining_characters);
        connect(this->_message, SIGNAL(textChanged(QString const&)),
                this, SLOT(_update_remaining_characters(QString const&)));
      }
      QVBoxLayout* vlayout = new QVBoxLayout(this);
      vlayout->setSpacing(0);
      vlayout->setContentsMargins(0, 0, 0, 0);
      vlayout->addWidget(this->_top_separator);
      QHBoxLayout* layout = new QHBoxLayout;
      layout->setSpacing(0);
      layout->setContentsMargins(16, 0, 13, 0);
      layout->addWidget(&this->_icon);
      layout->addSpacing(9);
      layout->addWidget(this->_message, 1);
      layout->addSpacing(2);
      layout->addWidget(&this->_remaining_characters, 0, Qt::AlignHCenter);
      vlayout->addLayout(layout);
      this->adjustSize();
    }

    QString
    Message::text() const
    {
      return this->_message->text();
    }

    void
    Message::_update_remaining_characters(QString const& text)
    {
      this->_remaining_characters.setText(
        QString("%1").arg(this->_maximum_characters - text.length()));
    }

    void
    Message::clear()
    {
      this->_message->clear();
    }

    QSize
    Message::sizeHint() const
    {
      return QSize(320, 41); //Super::sizeHint().height());
    }

  }
}
