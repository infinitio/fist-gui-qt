#include <fist-gui-qt/notification/LinkReady.hh>
#include <fist-gui-qt/notification/ui.hh>

#include <fist-gui-qt/model/Link.hh>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPushButton>
#include <QLabel>

namespace fist
{
  namespace notification
  {
    LinkReady::LinkReady(model::Link const& link,
                         QWidget* parent)
      : Super("",
              link_ready::view::body::text,
              2200,
              QPixmap(),
              parent)
      , _link(link)
    {
      link_ready::view::body::style(*this->_body);
    }
  }
}
