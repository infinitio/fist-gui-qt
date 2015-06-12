#include <fist-gui-qt/gui/EnterKeySequence.hh>

#include <QKeyEvent>

namespace fist
{
  namespace gui
  {
    EnterKeySequence::EnterKeySequence(QString const& text,
                                       QWidget *parent)
      : QLineEdit(text, parent)
      , _previous(text)
      , _sequence(text)
    {
      this->setEnabled(true);
    }

    void
    EnterKeySequence::keyPressEvent(QKeyEvent *e)
    {
      if (!this->isEnabled())
        return;
      e->accept();
      if (e->key() == Qt::Key_Escape)
        return;
      if (e->modifiers() == 0)
      {
        emit no_modifier();
        return;
      }
      if (!e->text().isEmpty())
      {
        this->_previous = this->_sequence;
        this->_sequence = QKeySequence(e->key() | e->modifiers());
        this->setText(this->_sequence.toString());
      }
    }

    void
    EnterKeySequence::rollback()
    {
      this->_sequence = this->_previous;
      this->setText(this->_sequence.toString());
    }
  }
}
