#include <fist-gui-qt/SendView/EmailResultWidget.hh>
#include <fist-gui-qt/gui/IconProvider.hh>

namespace fist
{
  namespace sendview
  {
    EmailResultWidget::EmailResultWidget(QString const& email,
                                         bool preselected,
                                         QWidget* parent)
     : Super(preselected, parent)
     , _email(email)
    {
      this->_avatar->set_avatar(QPixmap(":/icon/email"));
      this->_fullname->setText(this->_email);
    }

    void
    EmailResultWidget::_selected()
    {
      emit email_selected(this->_email);
    }

    void
    EmailResultWidget::_unselected()
    {
      emit email_unselected(this->_email);
    }
  }
}
