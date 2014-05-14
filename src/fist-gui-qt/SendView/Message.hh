#ifndef FIST_GUI_QT_SEND_VIEW_MESSAGE_HH
# define FIST_GUI_QT_SEND_VIEW_MESSAGE_HH

# include <QWidget>
# include <QLineEdit>
# include <QLabel>

# include <elle/attribute.hh>

namespace fist
{
  namespace sendview
  {
    class Message:
      public QWidget
    {
      typedef QWidget Super;
    public:
      Message(QWidget* parent,
                int maximum_characters = 100);

      ELLE_ATTRIBUTE_R(QLabel, icon);
      ELLE_ATTRIBUTE_R(QLineEdit*, message);
      ELLE_ATTRIBUTE(int, maximum_characters);
      ELLE_ATTRIBUTE_R(QLabel, remaining_characters);

    protected slots:
      void
      _update_remaining_characters(QString const&);

    public:
      QString
      text() const;

public slots:
void
clear();
    private:
      QSize
      sizeHint() const override;

    private:
      Q_OBJECT;
    };
  }
}



#endif
