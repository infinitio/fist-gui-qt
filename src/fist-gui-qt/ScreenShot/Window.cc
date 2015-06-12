#include <fist-gui-qt/ScreenShot/Window.hh>
#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/utils.hh>

#include <elle/log.hh>

#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>

ELLE_LOG_COMPONENT("fist.screenshot.Window");

namespace fist
{
  namespace screenshot
  {

    RegionSelector::RegionSelector(QWidget* parent)
      : Super(nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint// | Qt::X11BypassWindowManagerHint
        )
      , _process_selection(false)
      , _desktop_clear()
      , _desktop_background(QPixmap(QApplication::desktop()->size()))
    {
      this->grabKeyboard();
      this->show();
      this->setFocus();

      this->setWindowState(Qt::WindowFullScreen);
      this->setCursor(Qt::CrossCursor);

      this->setStyleSheet("background:transparent;");
      this->setAttribute(Qt::WA_TranslucentBackground);
      this->resize(QApplication::desktop()->size());
    }

    void RegionSelector::paintEvent(QPaintEvent * event)
    {
      if (this->_desktop_clear.isNull())
      {
        this->_draw_background();
        this->_desktop_clear = QPixmap::grabWindow(
          QApplication::desktop()->winId());
      }
      QPainter painter(this);
      painter.drawPixmap(QPoint(0, 0), this->_desktop_background);
      this->_draw_selection_rectangle(painter);
    }

    void RegionSelector::mousePressEvent(QMouseEvent* event)
    {
      if (event->button() != Qt::LeftButton)
        return;

      this->_selection_origin = event->pos();
      this->_process_selection = true;
    }

    void RegionSelector::mouseReleaseEvent(QMouseEvent* event)
    {
      if (event->button() != Qt::LeftButton)
        return;

      this->_selection_end = event->pos();
      this->_process_selection = false;
      this->selection = this->_desktop_clear.copy(this->_selection_rectangle);
      emit done();
    }

    void RegionSelector::mouseMoveEvent(QMouseEvent *event)
    {
      if (this->_process_selection)
      {
        this->_selection_end = event->pos();
        this->_selection_rectangle = QRect(_selection_origin, _selection_end).normalized();
        update();
      }
    }

    void RegionSelector::keyPressEvent(QKeyEvent* event)
    {
      if (event->key() == Qt::Key_Escape)
      {
        this->close();
        this->hide();
      }
      else
        event->ignore();
    }

    void RegionSelector::_draw_background()
    {
      QPainter painter(&this->_desktop_background);

      painter.setBrush(QBrush(QColor(0, 0, 0, 85), Qt::SolidPattern));
      painter.drawRect(QApplication::desktop()->rect());

      QRect txtRect = QApplication::desktop()->screenGeometry(
        QApplication::desktop()->primaryScreen());
      QString txtTip = QApplication::tr("Select a region.");
      txtRect.setHeight(qRound((float) (txtRect.height() / 10)));

      painter.setPen(QPen(Qt::red));
      painter.setBrush(QBrush(QColor(255, 255, 255, 180), Qt::SolidPattern));
      QRect txtBgRect = painter.boundingRect(txtRect, Qt::AlignCenter, txtTip);

      txtBgRect.setX(txtBgRect.x() - 6);
      txtBgRect.setY(txtBgRect.y() - 4);
      txtBgRect.setWidth(txtBgRect.width() + 12);
      txtBgRect.setHeight(txtBgRect.height() + 8);

      painter.drawRect(txtBgRect);

      // Draw the text
      painter.setPen(QPen(Qt::black)); // black color pen
      painter.drawText(txtBgRect, Qt::AlignCenter, txtTip);

      // set bkg to pallette widget
      QPalette newPalette = palette();
      newPalette.setBrush(QPalette::Window, QBrush(this->_desktop_background));
      setPalette(newPalette);
    }

    void RegionSelector::_draw_selection_rectangle(QPainter &painter)
    {
      if (!this->_desktop_clear.isNull())
      {
        painter.drawPixmap(this->_selection_rectangle, this->_desktop_clear, this->_selection_rectangle);
        painter.setPen(QPen(QBrush(QColor(127, 0, 0, 255)), 2));
        painter.drawRect(this->_selection_rectangle);

        QString txtSize = QApplication::tr("%1 x %2 pixels").arg(this->_selection_rectangle.width()).arg(this->_selection_rectangle.height());
        painter.drawText(this->_selection_rectangle, Qt::AlignBottom | ((this->_selection_origin.x() < this->_selection_rectangle.right()) ? Qt::AlignLeft : Qt::AlignRight), txtSize);

        if (fist::settings()["screenshot"].exists("zoom"))
        {
          const quint8 zoomSide = 120;

          QPoint zoomStart = _selection_end;
          zoomStart -= QPoint(zoomSide/5, zoomSide/5);

          QPoint zoomEnd = this->_selection_end;
          zoomEnd += QPoint(zoomSide/5, zoomSide/5);

          QRect zoomRect = QRect(zoomStart, zoomEnd);
          QPixmap zoomPixmap = this->_desktop_clear.copy(zoomRect).scaled(QSize(zoomSide, zoomSide), Qt::KeepAspectRatio);

          QPainter zoomPainer(&zoomPixmap);
          zoomPainer.setPen(QPen(QBrush(QColor(255, 0, 0, 180)), 2));
          zoomPainer.drawRect(zoomPixmap.rect()); // draw
          zoomPainer.drawText(zoomPixmap.rect().center() - QPoint(4, -4), "+");

          // position for drawing preview
          QPoint zoomCenter = this->_selection_rectangle.bottomRight();

          if (zoomCenter.x() + zoomSide > this->_desktop_clear.rect().width()
              || zoomCenter.y() + zoomSide > this->_desktop_clear.rect().height())
            zoomCenter -= QPoint(zoomSide, zoomSide);
          painter.drawPixmap(zoomCenter, zoomPixmap);
        }
      }
    }
  }
}
