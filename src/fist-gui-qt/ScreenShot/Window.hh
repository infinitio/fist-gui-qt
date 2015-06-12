#ifndef FIST_GUI_QT_SCREENSHOT_SELECTOR
# define FIST_GUI_QT_SCREENSHOT_SELECTOR

# include <QMouseEvent>
# include <QPainter>
# include <QWidget>
# include <QMainWindow>
# include <QSize>
# include <QPoint>

# include <elle/attribute.hh>

namespace fist
{
  namespace screenshot
  {
    class RegionSelector
      : public QMainWindow
    {
      typedef QMainWindow Super;
    public:
      RegionSelector(QWidget *parent = 0);
      virtual ~RegionSelector() = default;

    protected:
      void
      paintEvent(QPaintEvent *event) override;
      void
      mousePressEvent(QMouseEvent *event) override;
      void
      mouseReleaseEvent(QMouseEvent *event) override;
      void
      mouseMoveEvent(QMouseEvent *event) override;
      void
      keyPressEvent(QKeyEvent *event) override;
    signals:
      void
      done();
    private:
      ELLE_ATTRIBUTE(bool, process_selection);
      ELLE_ATTRIBUTE(QPixmap, desktop_clear);
      ELLE_ATTRIBUTE(QPixmap, desktop_background);

      ELLE_ATTRIBUTE(QRect, selection_rectangle);
      ELLE_ATTRIBUTE(QPoint, selection_origin);
      ELLE_ATTRIBUTE(QPoint, selection_end);

    public:
      QPixmap selection;

    private:
      void _draw_background();
      void _draw_selection_rectangle(QPainter &painter);

    private:
      Q_OBJECT
    };
  }
}

#endif
