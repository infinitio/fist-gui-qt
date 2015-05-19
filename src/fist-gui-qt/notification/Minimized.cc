#include <fist-gui-qt/notification/Minimized.hh>
#include <fist-gui-qt/notification/ui.hh>

#include <QVBoxLayout>
#include <QMovie>
#include <QLabel>
#include <QHBoxLayout>

namespace fist
{
  namespace notification
  {
    Minimized::Minimized(QWidget* parent)
      : Super(2500, parent)
    {
      auto* layout = new QHBoxLayout(this);
      {
        auto* icon = new QLabel(this);
        icon->setMovie(new QMovie(":/notification/minimized", QByteArray(), this));
        icon->movie()->start();
        layout->addWidget(icon, 0, 0);
      }
      {
        auto* vlayout = new QVBoxLayout;
        {
          auto* label = new QLabel("Infinit is minimized!", this);
          update_available::view::title::style(*label);
          vlayout->addWidget(label);
        }
        {
          auto* label = new QLabel("Make sure the Infinit icon is always visible by clicking customize!", this);
          update_available::view::body::style(*label);
          vlayout->addWidget(label);
        }
        layout->addLayout(vlayout);
      }
    }
  }
}
