#ifndef FIST_GUI_QT_MAINVIEW_EMPTYSTATE_HH
# define FIST_GUI_QT_MAINVIEW_EMPTYSTATE_HH

# include <QVBoxLayout>
# include <QLabel>

# include <fist-gui-qt/ListItem.hh>

# include <elle/attribute.hh>

namespace fist
{
  namespace mainview
  {
    class EmptyState
      : public ListItem
    {
      typedef ListItem Super;
    public:
      EmptyState(QString const& icon,
                 QString const& body,
                 QString const& text,
                 QWidget* parent = nullptr,
                 bool show_link = true);

    private:
      void
      trigger() override {};

    protected:
      virtual
      void
      _run_tutorial()
      {
      }

    private:
      QSize
      sizeHint() const override
      {
        auto size = this->_layout->minimumSize();
        return QSize(Super::sizeHint().width(), size.height());
      }

      void
      paintEvent(QPaintEvent* event) override;

      bool
      eventFilter(QObject* obj, QEvent* event) override;

      ELLE_ATTRIBUTE(QVBoxLayout*, layout);
      ELLE_ATTRIBUTE(QLabel*, label);
      ELLE_ATTRIBUTE(QLabel*, link);
    };
  }
}

#endif
