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
    class _SearchResultWidget
      : public ListItem
    {
      typedef ListItem Super;
    protected:
      _SearchResultWidget(bool preselected = false,
                         QWidget* parent = nullptr);
      virtual
      ~_SearchResultWidget() = default;

    private:
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

    protected slots:
      virtual
      void
      _selected() = 0;

      virtual
      void
      _unselected() = 0;
/*-----------.
  | Attributes |
  `-----------*/
    protected:
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

    class SearchResultWidget
      : public _SearchResultWidget
    {
      typedef _SearchResultWidget Super;
    public:
      SearchResultWidget(fist::model::User const& model,
                         bool preselected,
                         QWidget* parent);

      ~SearchResultWidget() = default;
    protected:
      fist::model::User const& _model;

    private slots:
      void      _on_avatar_updated();


    protected slots:
      void
      _selected();

      void
      _unselected();

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
