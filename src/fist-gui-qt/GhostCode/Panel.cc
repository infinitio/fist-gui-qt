#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QMovie>
#include <QPainter>
#include <QVBoxLayout>

#include <fist-gui-qt/GhostCode/Panel.hh>
#include <fist-gui-qt/GhostCode/Footer.hh>
#include <fist-gui-qt/GhostCode/ui.hh>

namespace fist
{
  namespace ghost_code
  {
    Panel::Panel(fist::State& state)
      : ::Panel(new Footer)
      , _state(state)
      , _loading_icon(new QMovie(QString(":/loading"), QByteArray(), this))
      , _message(new QLabel(this))
      , _field(new QLineEdit(this))
    {
      // Loading.
      {
        this->_message->setMovie(this->_loading_icon);
        this->_message->movie()->start();
        this->_message->hide();
      }
      // Field.
      {
        view::code_field::style.font().setLetterSpacing(
          QFont::AbsoluteSpacing, 10);
        view::code_field::style(*this->_field);
        this->_field->setMaxLength(5);
        this->_field->setTextMargins(25, 0, 0, 0);
        this->_field->setFixedSize(160, 70);
        connect(this->_field, SIGNAL(textEdited(QString const&)),
                this, SLOT(_text_udpated(QString const&)));
      }
      // Footer.
      {
        this->footer()->next()->setDisabled(true);
      }
      // Buttons.
      {
        connect(this->footer()->next(), SIGNAL(released()),
                this, SLOT(_use_code()));
      }
      // Result.
      {
        connect(&this->_state, SIGNAL(ghost_code_result(gap_Status)),
                this, SLOT(_ghost_code_result(gap_Status)));
      }
      auto* body = new QWidget(this);
      body->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
      QVBoxLayout* layout = new QVBoxLayout(body);
      layout->setSpacing(15);
      {
        auto* logo = new QLabel(this);
        logo->setPixmap(QPixmap(":/logo").scaled(64, 64));
        layout->addWidget(logo, 0, Qt::AlignCenter);
      }
      QLabel* invited = new QLabel(view::title::text, this);
      view::title::style(*invited);
      layout->addWidget(invited, 0, Qt::AlignCenter);
      layout->addSpacing(5);
      QLabel* enter_code = new QLabel(view::subtitle::text, this);
      view::subtitle::style(*enter_code);
      layout->addWidget(enter_code, 0, Qt::AlignCenter);
      layout->addSpacing(5);
      layout->addWidget(this->_message, 0, Qt::AlignCenter);
      layout->addSpacing(5);
      {
        QHBoxLayout* hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 0);
        hlayout->setSpacing(0);
        hlayout->addStretch();
        {
          auto* left = new QLabel(this);
          left->setPixmap(QPixmap(":/code/left"));
          hlayout->addWidget(left);
        }
        hlayout->addWidget(this->_field);
        {
          auto* right = new QLabel(this);
          right->setPixmap(QPixmap(":/code/right"));
          hlayout->addWidget(right);
        }
        hlayout->addStretch();
        layout->addLayout(hlayout);
      }
      layout->addSpacing(45);
      layout->addStretch();

      QPalette palette = this->palette();
      {
        palette.setColor(QPalette::Window, QColor(view::background::color));
      }
      this->setPalette(palette);
      this->setAutoFillBackground(true);
      emit set_background_color(view::background::color);
    }

    void
    Panel::showEvent(QShowEvent* event)
    {
      ::Panel::showEvent(event);
      emit set_background_color(view::background::color);
      this->_field->setFocus();
    }

    void
    Panel::hideEvent(QHideEvent* event)
    {
      this->_field->clear();
      this->_message->clear();
      this->_message->hide();
      this->footer()->next()->setDisabled(true);
      ::Panel::hideEvent(event);
    }

    void
    Panel::keyPressEvent(QKeyEvent* event)
    {
      if (event->key() == Qt::Key_Escape)
        this->footer()->skip()->click();
      else if (event->key() == Qt::Key_Return)
        this->footer()->next()->click();
      ::Panel::keyPressEvent(event);
    }

    void
    Panel::_text_udpated(QString const& text)
    {
      if (text.length() == this->_field->maxLength())
      {
        this->footer()->next()->setDisabled(false);
        this->footer()->next()->setFocus();
      }
      else
      {
        this->footer()->next()->setDisabled(true);
      }
    }

    void
    Panel::_use_code()
    {
      this->_message->clear();
      this->_message->setMovie(this->_loading_icon);
      this->_message->movie()->start();
      this->_message->show();
      this->_state.use_ghost_code(this->_field->text().toStdString(),
                                  true /* manual */);
      this->footer()->next()->setDisabled(true);
    }

    void
    Panel::_ghost_code_result(gap_Status status)
    {
      this->footer()->next()->setDisabled(
        this->_field->text().length() != this->_field->maxLength());
      if (status == gap_ok)
        this->footer()->skip()->click();
      else if (status == gap_ghost_code_already_used)
        this->_message->setText("You already used that code");
      else if (status == gap_unknown_user)
        this->_message->setText("Invalid code");
    }

    ghost_code::Footer*
    Panel::footer()
    {
      return static_cast<Footer*>(this->_footer);
    }
  }
}
