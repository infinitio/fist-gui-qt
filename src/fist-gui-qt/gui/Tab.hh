#ifndef FIST_GUI_QT_GUI_TAB_HH
# define FIST_GUI_QT_GUI_TAB_HH

# include <QColor>
# include <QFont>
# include <QLabel>
# include <QObject>
# include <QPushButton>
# include <QVector>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

namespace fist
{
  namespace gui
  {
    class TabWidget;

    class Tab
      : public QWidget
      , public elle::Printable
    {
      typedef QWidget Super;

    public:
      Tab(TabWidget& tabber,
          QString const& name,
          QVector<QWidget*> const& widgets);

      virtual
      ~Tab() = default;

    private:
      void
      enterEvent(QEvent* event) override;

      void
      leaveEvent(QEvent* event) override;

      void
      paintEvent(QPaintEvent* event) override;

      void
      mouseReleaseEvent(QMouseEvent* event) override;

      QSize
      sizeHint() const override;

      QSize
      minimumSizeHint() const override;

    public slots:
      void
      click();

    private:
      void
      _hover();

      void
      _active();

      void
      _inactive();

    private slots:
      void
      _notify_tabber();

    public slots:
      void
      enable();

      void
      disable();

    public slots:
      void
      on_notification_count_changed(size_t count);

    signals:
      void
      activated();

    private:
      void
      print(std::ostream& stream) const override;

    private:
      ELLE_ATTRIBUTE(TabWidget&, tabber);
      ELLE_ATTRIBUTE_R(QLabel*, name)
      ELLE_ATTRIBUTE_R(size_t, notification_count);
      ELLE_ATTRIBUTE(QLabel*, counter)
      ELLE_ATTRIBUTE(QVector<QWidget*>, widgets);
      ELLE_ATTRIBUTE_Rw(QColor, color);
    private:
      Q_OBJECT;

    private:
      friend TabWidget;
    };
  }
}

#endif
