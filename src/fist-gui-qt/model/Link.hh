#ifndef FIST_GUI_QT_MODEL_LINK_HH
# define FIST_GUI_QT_MODEL_LINK_HH

# include <QObject>
# include <QString>
# include <QDateTime>
# include <QUrl>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

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
      ELLE_ATTRIBUTE_R(QUrl, url);
      ELLE_ATTRIBUTE_R(QString, name);
      ELLE_ATTRIBUTE_R(QDateTime, mtime);
      ELLE_ATTRIBUTE_R(uint32_t, download_count);
      ELLE_ATTRIBUTE_R(float, progress);

    private:
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
