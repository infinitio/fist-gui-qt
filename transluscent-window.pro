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
LIBS += -Wl,-rpath-link,../ clib/
LIBS += -lgap

# Input
SOURCES +=                                      \
    AvatarWidget.cc                             \
    AvatarIcon.cc                               \
    DebugWindow.cc                              \
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
    TransactionPanel.cc                         \
    TransactionWidget.cc                        \
    TransactionWindow.cc                        \
    UserWidget.cc                               \
    main.cc                                     \

HEADERS +=                                      \
    AvatarWidget.hh                             \
    AvatarIcon.hh                               \
    DebugWindow.hh                              \
    Footer.hh                                   \
    IconButton.hh                               \
    InfinitDock.hh                              \
    ListItem.cc                                 \
    ListWidget.hh                               \
    LoginWindow.hh                              \
    RoundShadowWidget.hh                        \
    SearchField.hh                              \
    SendFooter.hh                               \
    SendPanel.hh                                \
    SmoothLayout.hh                             \
    SmoothScrollBar.hh                          \
    TransactionFooter.hh                        \
    TransactionList.hh                          \
    TransactionPanel.hh                         \
    TransactionWidget.hh                        \
    TransactionWindow.hh                        \
    UserWidget.hh                               \

RESOURCES += resources/resources.qrc

QMAKE_CXXFLAGS += -std=c++11

FORMS += \
    LoginWindow.ui
