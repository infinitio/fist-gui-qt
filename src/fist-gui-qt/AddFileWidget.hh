#ifndef ADDFILEITEM_HH
# define ADDFILEITEM_HH

# include <QLabel>
# include <QWidget>
# include <QColor>

# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/utils.hh>

class AddFileWidget:
  public QWidget
{
public:
  AddFileWidget(QWidget* parent = nullptr);

  virtual
  QSize
  sizeHint() const override;

  IconButton*
  attach();

public:
  Q_PROPERTY(QColor pulseColor
             READ pulseColor
             WRITE setPulseColor);
  Q_PROPERTY_R(QColor, pulse_color, pulseColor);

  void
  setPulseColor(QColor const& color);

public slots:
  void
  pulse();

private:
  QLabel* _text;
  IconButton* _attach;

private:
  Q_OBJECT;
};

#endif
