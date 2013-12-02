#include <fist-gui-qt/SendFooter.hh>

#include <iostream>
SendFooter::SendFooter():
  Footer(nullptr),
  _back(new IconButton(QPixmap(":/icons/trash.png"),
                       true,
                       [this]
                       {
                         std::cerr << "back" << std::endl;
                         emit toggle_transaction_view();
                       })),
  _attach(new IconButton(QPixmap(":/icons/clip.png"),
                         true,
                         [this]
                         {
                           std::cerr << "attach" << std::endl;
                           emit toggle_file_picker();
                         }))

{
  this->_layout->addWidget(this->_back);
  this->_layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::MinimumExpanding,
                                  QSizePolicy::Minimum));
  this->_layout->addWidget(this->_attach);
}
