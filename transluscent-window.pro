######################################################################
# Automatically generated by qmake (2.01a) Thu Aug 1 14:23:52 2013
######################################################################

CONFIG += debug
DEPENDPATH += .
INCLUDEPATH += . ../infinit/ ../infinit/_build/linux64/
TEMPLATE = app
TARGET =

LIBS += -L ../infinit/_build/linux64/lib/
LIBS += -Wl,-rpath-link,../infinit/_build/linux64/lib/
LIBS += -lgap

# Input
SOURCES +=                                      \
    AvatarWidget.cc                             \
    DebugWindow.cc                              \
    Footer.cc                                   \
    IconButton.cc                               \
    InfinitDock.cc                              \
    ListItem.cc                                 \
    ListWidget.cc                               \
    LoginWindow.cc                              \
    RoundShadowWidget.cc                        \
    SearchField.cc                              \
    SendPanel.cc                                \
    SmoothLayout.cc                             \
    SmoothScrollBar.cc                          \
    TransactionList.cc                          \
    TransactionPanel.cc                         \
    TransactionWidget.cc                        \
    TransactionWindow.cc                        \
    UserWidget.cc                               \
    main.cc                                     \

HEADERS +=                                      \
    AvatarWidget.hh                             \
    DebugWindow.hh                              \
    Footer.hh                                   \
    IconButton.hh                               \
    InfinitDock.hh                              \
    ListItem.cc                                 \
    ListWidget.hh                               \
    LoginWindow.hh                              \
    RoundShadowWidget.hh                        \
    SearchField.hh                              \
    SendPanel.hh                                \
    SmoothLayout.hh                             \
    SmoothScrollBar.hh                          \
    TransactionList.hh                          \
    TransactionPanel.hh                         \
    TransactionWidget.hh                        \
    TransactionWindow.hh                        \
    UserWidget.hh                               \

RESOURCES += resources/resources.qrc

QMAKE_CXXFLAGS += -std=c++11
