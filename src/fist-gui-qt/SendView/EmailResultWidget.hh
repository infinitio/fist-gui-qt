#ifndef FIST_GUI_QT_SEND_VIEW_EMAILRESULTWIDGET_HH
# define FIST_GUI_QT_SEND_VIEW_EMAILRESULTWIDGET_HH

# include <QHBoxLayout>
# include <QLabel>

# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/model/User.hh>
# include <fist-gui-qt/TwoStateIconButton.hh>
# include <fist-gui-qt/SendView/SearchResultWidget.hh>

namespace fist
{
  namespace sendview
  {
    class EmailResultWidget
      : public _SearchResultWidget
    {
      typedef _SearchResultWidget Super;
    public:
      EmailResultWidget(QString const& email,
                        bool preselected,
                        QWidget* parent = nullptr);


    signals:
      void
      email_selected(QString const&);

      void
      email_unselected(QString const&);

    private slots:
      void
      _selected() override;

      void
      _unselected() override;

    private:
      ELLE_ATTRIBUTE_R(QString, email);
    private:
      Q_OBJECT;
    };
  }
}

#endif
