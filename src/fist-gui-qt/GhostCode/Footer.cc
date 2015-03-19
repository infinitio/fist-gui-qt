#include <elle/printf.hh>

#include <fist-gui-qt/GhostCode/Footer.hh>

namespace fist
{
  namespace ghost_code
  {
    Footer::Footer()
      : Super()
      , _skip(new QPushButton("SKIP", this))
      , _next(new QPushButton("NEXT", this))
    {
      auto style = QString::fromStdString(
        elle::sprintf(
          "QPushButton {"
          "  background-color: rgb(248, 93, 91);"
          "  border-radius: 0px;"
          "  color: white;"
          "  font: bold 13px;"
          "  width: 80px;"
          "  height: %spx;"
          "}"
          "QPushButton:focused, QPushButton:hover {"
          "  color: white;"
          "  background-color: rgb(228, 73, 71);"
          "}"
          "QPushButton:disabled, QPushButton:hover::disabled {"
          "  color: rgb(248, 175, 173);"
          "  background-color: rgb(248, 93, 91);"
          "}",
          this->height()));
      this->_layout->setContentsMargins(0, 0, 0, 0);
      this->_skip->setStyleSheet(style);
      this->_layout->addWidget(this->_skip);
      this->_layout->addStretch();
      this->_next->setStyleSheet(style);
      this->_layout->addWidget(this->_next);
    }
  }
}
