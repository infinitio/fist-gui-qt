#ifndef FIST_GUI_TOOLTIP_HH
# define FIST_GUI_TOOLTIP_HH

# include <QWidget>
# include <QMainWindow>
# include <QTimer>
# include <QPropertyAnimation>

# include <elle/attribute.hh>
 # include <elle/Printable.hh>

namespace fist
{
  namespace gui
  {
    // Tooltip is a small class to replace native tooltips giving more
    // flexibility.
    // The tooltip is a tool mainwindow.
    // The tooltip needs an anchor, whic is the widget it's attached to.
    // The aligment set the position of the tooltip from his anchor.
    // XXX: For the moment, only Left & Right are implemented.
    // XXX: The tooltip doesn't handle widget outside of the applicaiton
    // viewport (understand: clipped widget).
    class Tooltip:
      public QMainWindow,
      public elle::Printable
    {
      typedef QMainWindow Super;
    public:
      // Construct a tooltip with:
      // - text: The message in the tooltip.
      // - anchor: The  the widget is anchored with.
      // - parent: To manage ownership.
      // - aligmnent: The alignment of the tooltip from his anchored widget.
      // - duration: The duration until the tooltip fades.
      // - margin: The margin of the text inside the widget, doubled for left
      //           and right.
      Tooltip(QString const& text,
              QWidget* widget,
              QWidget* parent = nullptr,
              Qt::AlignmentFlag alignment = Qt::AlignLeft,
              int duration = 5000,
              int margin = 3);

      virtual
      ~Tooltip();

      // Recompute tooltip position.
      void
      refresh_position();

      // Filter anchor events to detect changes like visibility changed,
      // position changed, modal window spawned...
      bool
      eventFilter(QObject *obj, QEvent *event) override;

      // Draw the pretty bubble.
      void
      paintEvent(QPaintEvent*) override;

      // Activate the fading effect on click.
      void
      mouseReleaseEvent(QMouseEvent * event) override;

    private:
      ELLE_ATTRIBUTE(QString, text);
      ELLE_ATTRIBUTE(QWidget*, widget);
      ELLE_ATTRIBUTE(int, margin);
      ELLE_ATTRIBUTE(Qt::AlignmentFlag, alignment);
      ELLE_ATTRIBUTE(QTimer*, fade_timer);
      ELLE_ATTRIBUTE(int, duration_until_fade);

      Q_PROPERTY(float opacity READ opacity WRITE opacity NOTIFY opacityChanged);

      void
      enterEvent(QEvent* event) override;

      void
      leaveEvent(QEvent* event) override;

      void
      showEvent(QShowEvent* event) override;

      void
      hideEvent(QHideEvent* event) override;

    public slots:
      // Disable the tooltip.
      void
      over();

    private slots:
      // Called if the anchored widget is deleted.
      void
      _anchored_widget_deleted();

    private:
      ELLE_ATTRIBUTE(bool, finished);

    public:
      ELLE_ATTRIBUTE_Rw(float, opacity);
      ELLE_ATTRIBUTE(QPropertyAnimation*, fade_animation);
      ELLE_ATTRIBUTE(QPropertyAnimation*, show_animation);
      ELLE_ATTRIBUTE(int, blocked_by_a_modal_window);
    Q_SIGNALS:
      void
      opacityChanged();

    signals:
      void
      clicked();

      void
      hidden();
    private:
      /*----------.
      | Printable |
      `----------*/
      void
      print(std::ostream& stream) const override;

    private:
      Q_OBJECT;
    };
  }
}

#endif
