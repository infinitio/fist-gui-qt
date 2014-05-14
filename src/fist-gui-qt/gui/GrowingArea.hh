#ifndef FIST_GUI_QT_GUI_GROWINGAREA_HH
# define FIST_GUI_QT_GUI_GROWINGAREA_HH

# include <QPropertyAnimation>
# include <QWidget>

# include <elle/attribute.hh>

namespace fist
{
  namespace gui
  {
    // GrowingArea wrap a widget and make it expandable and shrinkable.
    // The expand method make it grow up the the wrapped widget height.
    // The shrink method reduce the height down to 0.
    class GrowingArea:
      public QWidget
    {
      typedef QWidget Super;
    public:

      enum class State
      {
        expanding,
        expanded,
        shrinking,
        shrinked
      };

      GrowingArea(QWidget* body,
                  QWidget* parent = nullptr);

    private:
      bool
      eventFilter(QObject *obj, QEvent *event) override;

    public slots:
      void
      expand(bool force = false);

      void
      shrink();

    private slots:
      void
      _done();

    private:
      Q_PROPERTY(int minimumHeightHint
                 READ minimumHeightHint
                 WRITE minimumHeightHint
                 NOTIFY minimumHeightHintChanged);

    public:
      ELLE_ATTRIBUTE_R(QWidget*, body);
      ELLE_ATTRIBUTE_Rw(int, minimumHeightHint);
      ELLE_ATTRIBUTE(QPropertyAnimation*, height_animator);
      ELLE_ATTRIBUTE(State, state);

    public:
      QSize
      minimumSizeHint() const override;
      QSize
      sizeHint() const override;

    signals:
      void
      minimumHeightHintChanged();
      void
      shrinking();
      void
      expanding();
      void
      shrinked();
      void
      expanded();

    private:
      Q_OBJECT;
    };
  }
}


#endif
