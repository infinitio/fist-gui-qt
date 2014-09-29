#ifndef FIST_GUI_QT_MODEL_LINK_HH
# define FIST_GUI_QT_MODEL_LINK_HH

# include <QObject>
# include <QString>
# include <QDateTime>
# include <QUrl>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <surface/gap/gap.hh>

# include <fist-gui-qt/model/Model.hh>
# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace model
  {
    class Link
      : public Model
    {
      typedef Model Super;
    public:
      Link(fist::State& state,
           uint32_t id);
      virtual
      ~Link() = default;

    private:
      ELLE_ATTRIBUTE_P(surface::gap::LinkTransaction, link, mutable);
      ELLE_ATTRIBUTE_P(QDateTime, mtime, mutable);
      ELLE_ATTRIBUTE_P(bool, final, mutable);

    public:
      void
      update();

    public:
      QUrl
      url() const;

      QString
      name() const;

      QDateTime const&
      mtime() const;

      uint32_t
      click_count() const;

      float
      progress() const;

      gap_TransactionStatus
      status() const;

      bool
      unavailable() const;

      bool
      is_finished() const;

    signals:
      void
      status_updated() const;

      void
      click_count_updated() const;

    private:
      /*----------.
      | Printable |
      `----------*/
      void
      print(std::ostream& stream) const override;

    private:
      // Make State friend in order to allow it to send signals.
      friend fist::State;

    private:
      Q_OBJECT;
    };
  }
}

#endif
