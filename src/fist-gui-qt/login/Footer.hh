#ifndef FIST_GUI_QT_LOGIN_FOOTER_HH
# define FIST_GUI_QT_LOGIN_FOOTER_HH

# include <QLabel>
# include <QMovie>

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/login/fwd.hh>

# include <elle/attribute.hh>

namespace fist
{
  namespace login
  {
    class Footer:
      public ::Footer
    {
      typedef ::Footer Super;
    public:
      Footer(QWidget* parent = nullptr);

      virtual
      ~Footer() = default;

    Q_SIGNALS:
      void
      login();

      void
      register_();

    public:
      void
      click();

    private:
      void
      mouseReleaseEvent(QMouseEvent * event) override;

      void
      enterEvent(QEvent* event) override;

      void
      leaveEvent(QEvent* event) override;

    private:
      ELLE_ATTRIBUTE(QMovie*, loading);
      ELLE_ATTRIBUTE(QLabel*, text);
      ELLE_ATTRIBUTE_Rw(Mode, mode);
    private:
      Q_OBJECT
    };
  }
}

#endif
