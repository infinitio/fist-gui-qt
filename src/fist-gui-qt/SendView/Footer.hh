#ifndef FIST_GUI_QT_SEND_VIEW_FOOTER_HH
# define FIST_GUI_QT_SEND_VIEW_FOOTER_HH

# include <elle/attribute.hh>

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/ShapeButton.hh>

# include <QHBoxLayout>
# include <QSpacerItem>


namespace fist
{
  namespace sendview
  {

    class Footer:
      public ::Footer
    {
    public:
      Footer();

      virtual
      ~Footer() = default;

    public slots:
      void
      peer_transaction_mode();
      void
      link_mode();

    private:
      ELLE_ATTRIBUTE_R(IconButton*, back);
      ELLE_ATTRIBUTE_R(IconButton*, send);

    private:
      Q_OBJECT;
    };

  }
}

#endif
