# include <fist-gui-qt/notification/UpdateAvailable.hh>
# include <fist-gui-qt/notification/ui.hh>

namespace fist
{
  namespace notification
  {
    UpdateAvailable::UpdateAvailable(QWidget* parent)
      : Super(update_available::view::title::text,
              update_available::view::body::text.arg(""),
              2500,
              QPixmap(":/notification/update"),
              parent)
    {
      update_available::view::title::style(*this->_title);
      update_available::view::body::style(*this->_body);
    }
  }
}
