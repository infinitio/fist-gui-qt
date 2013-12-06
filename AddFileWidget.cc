#include <fist-gui-qt/AddFileWidget.hh>

#include <QPixmap>
#include <QHBoxLayout>

AddFileWidget::AddFileWidget(QWidget* parent):
  QWidget(parent),
  _text(new QLabel(QString("attach files"))),
  _attach(new IconButton(QPixmap(":/icons/clip.png"), // Remove.
                         true))
{
  auto* layout = new QHBoxLayout(this);
  layout->addWidget(this->_attach);
  layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::Minimum, QSizePolicy::Maximum));
  layout->addWidget(this->_text);
  this->setFixedWidth(320);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
};

IconButton*
AddFileWidget::attach()
{
  return this->_attach;
}

QSize
AddFileWidget::sizeHint() const
{
  return QSize(this->width(), 24);
}
