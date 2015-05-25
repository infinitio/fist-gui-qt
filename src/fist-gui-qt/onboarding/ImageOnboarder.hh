#ifndef FIST_GUI_QT_ONBOARDING_IMAGER_HH
# define FIST_GUI_QT_ONBOARDING_IMAGER_HH

# include <memory>
# include <vector>

# include <QWidget>
# include <QLabel>
# include <QString>
# include <QToolBar>
# include <QPushButton>

# include <fist-gui-qt/RoundShadowWidget.hh>

# include <elle/attribute.hh>

namespace fist
{
  namespace onboarding
  {
    class Dots
      : public QWidget
    {
public:
      Dots(int count,
            QWidget* parent);
    private:
      void
      paintEvent(QPaintEvent* event) override;

      ELLE_ATTRIBUTE_Rw(unsigned int, count);
      ELLE_ATTRIBUTE_RW(unsigned int, current);
      ELLE_ATTRIBUTE_R(int, diameter);
      ELLE_ATTRIBUTE_RW(int, margin);
    };

    class Landing
      : public QWidget
    {
      typedef QWidget Super;

    public:
      Landing(QWidget* parent);
    };

    class Panel
      : public QWidget
    {
      typedef QWidget Super;

    public:
      Panel(QString const& title,
            QString const& icon,
            QString const& body,
            QWidget* parent);

    private:
      void
      paintEvent(QPaintEvent*) override;

    private:
      QLabel* _title;
    public:
      QMovie* icon;
    };

    class ImageOnboarder
      : public QMainWindow
    {
      typedef QMainWindow Super;
    public:
      ImageOnboarder(QWidget* widget = nullptr);

    private:
      void
      _add(QWidget* image);

    private:
      void
      keyPressEvent(QKeyEvent* event) override;

    signals:
      void
      onboarded();

    private:
      ELLE_ATTRIBUTE(std::vector<QWidget*>, images);
      ELLE_ATTRIBUTE(QLabel*, current);
      ELLE_ATTRIBUTE(QPushButton*, prev);
      ELLE_ATTRIBUTE(Dots*, foo);
      ELLE_ATTRIBUTE(QPushButton*, next);
    public slots:
      void
      next(int i = 1);

      void
      prev();
    public:
      Q_OBJECT;
    };
  }
}

#endif
