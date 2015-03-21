#ifndef FIST_GUI_QT_SEND_VIEW_SEARCHRESULTWIDGET_HH
# define FIST_GUI_QT_SEND_VIEW_SEARCHRESULTWIDGET_HH

# include <QHBoxLayout>
# include <QLabel>

# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/model/User.hh>
# include <fist-gui-qt/TwoStateIconButton.hh>

namespace fist
{
  namespace sendview
  {
    class SearchResultWidget
      : public ListItem
    {
      typedef ListItem Super;
    public:
      SearchResultWidget(fist::model::User const& model,
                         bool preselected = false,
                         QWidget* parent = nullptr);
      virtual
      ~SearchResultWidget() = default;

      QSize
      sizeHint() const override;

      void
      enterEvent(QEvent*) override;

      void
      leaveEvent(QEvent*) override;

    Q_SIGNALS:
      void
      selected(uint32_t);
      void
      unselected(uint32_t);

    public:
      void
      trigger() override;

    protected:
      void
      _update() override;

    private slots:
      void
      _on_avatar_updated();

    protected slots:
      virtual
      void
      _selected();

      virtual
      void
      _unselected();
/*-----------.
  | Attributes |
  `-----------*/
    protected:
      fist::model::User const& _model;
      QLabel* _fullname;
      AvatarIcon* _avatar;
      ELLE_ATTRIBUTE_R(fist::TwoStateIconButton*, selector);

/*-------.
  | Layout |
  `-------*/
    private:
      QHBoxLayout* _layout;

/*----------.
  | Printable |
  `----------*/
      void
      print(std::ostream& stream) const override;

    private:
      Q_OBJECT;
    };
  }
}
#endif
