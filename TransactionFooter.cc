#include <fist-gui-qt/TransactionFooter.hh>

TransactionFooter::TransactionFooter():
  Footer(nullptr),
  _send(new IconButton(QPixmap(":/icons/arrows.png"),
                       true,
                       [this]
                       {
                         std::cerr << "send" << std::endl;
                         emit toggle_send_view();
                       }))
{
  this->_layout->addWidget(new IconButton(QPixmap(":/icons/gear.png"), true));
  this->_layout->addItem(new QSpacerItem(0, 0,
                                         QSizePolicy::MinimumExpanding,
                                         QSizePolicy::Minimum));
  this->_layout->addWidget(this->_send);
}
