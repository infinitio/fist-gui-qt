######################################################################
# Automatically generated by qmake (2.01a) Thu Aug 1 14:23:52 2013
######################################################################

QT += network
CONFIG += debug
DEPENDPATH += .
INCLUDEPATH += . ../ ../../../
TEMPLATE = app
TARGET =

LIBS += -L ../lib/
LIBS += -Wl,-rpath-link,../lib/
LIBS += -lgap

# Input
SOURCES +=                                      \
    AvatarWidget.cc                             \
    AvatarIcon.cc                               \
    DebugWindow.cc                              \
    FileItem.cc                                 \
    Footer.cc                                   \
    IconButton.cc                               \
    InfinitDock.cc                              \
    ListItem.cc                                 \
    ListWidget.cc                               \
    LoginWindow.cc                              \
    RoundShadowWidget.cc                        \
    SearchField.cc                              \
    SendFooter.cc                               \
    SendPanel.cc                                \
    SmoothLayout.cc                             \
    SmoothScrollBar.cc                          \
    TransactionFooter.cc                        \
    TransactionList.cc                          \
    TransactionModel.cc                         \
    TransactionPanel.cc                         \
    TransactionWidget.cc                        \
    TransactionWindow.cc                        \
    UserModel.cc                                \
    UserWidget.cc                               \
    main.cc                                     \

HEADERS +=                                      \
    AvatarWidget.hh                             \
    AvatarIcon.hh                               \
    DebugWindow.hh                              \
    FileItem.hh                                 \
    Footer.hh                                   \
    IconButton.hh                               \
    InfinitDock.hh                              \
    ListItem.hh                                 \
    ListWidget.hh                               \
    LoginWindow.hh                              \
    RoundShadowWidget.hh                        \
    SearchField.hh                              \
    SendFooter.hh                               \
    SendPanel.hh                                \
    SmoothLayout.hh                             \
    SmoothScrollBar.hh                          \
    TransactionFooter.hh                        \
    TransactionModel.hh                         \
    TransactionList.hh                          \
    TransactionPanel.hh                         \
    TransactionWidget.hh                        \
    TransactionWindow.hh                        \
    UserModel.hh                                \
    UserWidget.hh                               \

RESOURCES += resources/resources.qrc

QMAKE_CXXFLAGS += -std=c++11

FORMS += \
    LoginWindow.ui
