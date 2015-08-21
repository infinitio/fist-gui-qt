#ifndef FIST_GUI_QT_CONTAINERS_UPDATABLE_HH
# define FIST_GUI_QT_CONTAINERS_UPDATABLE_HH

# include <QObject>

namespace fist
{
  class UpdatetableContainer
    : public QObject
  {
  protected:
    UpdatetableContainer() = default;
    virtual
    ~UpdatetableContainer();

  signals:
    void
    size_changed(size_t);

  private:
    Q_OBJECT;
  };
}

#endif
