#include <fist-gui-qt/HorizontalSeparator.hh>

HorizontalSeparator::HorizontalSeparator(QWidget* parent):
  QFrame(parent)
{
  this->setFrameShape(QFrame::HLine);
  this->setFrameShadow(QFrame::Sunken);

  this->setFixedHeight(5);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}
