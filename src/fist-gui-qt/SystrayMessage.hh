#ifndef FIST_GUI_QT_SYSTRAYMESSAGE_HH
# define FIST_GUI_QT_SYSTRAYMESSAGE_HH

# include <memory>

# include <QObject>
# include <QSystemTrayIcon>
# include <QString>

# include <elle/attribute.hh>

namespace fist
{
  class Message
  {
  public:
    using Icon = QSystemTrayIcon::MessageIcon;

  public:
    Message(QString const& title,
            QString const& body,
            Icon const& icon = Icon::Information,
            int duration = 3000,
            bool always_show = false);
    virtual
    ~Message() = default;
  private:
    // The title of the message.
    ELLE_ATTRIBUTE_R(QString, title);
    // The body of the message.
    ELLE_ATTRIBUTE_R(QString, body);
    // The icon.
    ELLE_ATTRIBUTE_R(QSystemTrayIcon::MessageIcon, icon);
    // The duration in ms.
    ELLE_ATTRIBUTE_R(int, duration);
    // Show even if the dock is visible.
    ELLE_ATTRIBUTE_R(bool, always_show);
  };

  class UpdateAvailableMessage
    : public Message
  {
  public:
    UpdateAvailableMessage(
      QString const& title,
      QString const& body,
      Message::Icon const& icon = Message::Icon::Information,
      int duration = 3000,
      bool always_show = true);
    virtual
    ~UpdateAvailableMessage() = default;
  };

  class SystrayMessageCarrier
    : public QObject
  {
  public:
    SystrayMessageCarrier(Message* ptr)
      : body(ptr)
    {}

    virtual
    ~SystrayMessageCarrier() = default;
  public:
    mutable std::unique_ptr<Message> body;
  private:
    Q_OBJECT;
  };

}

#endif
