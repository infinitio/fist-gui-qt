#ifndef FIST_GUI_QT_MODEL_MODEL_HH
# define FIST_GUI_QT_MODEL_MODEL_HH

# include <QObject>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace model
  {
    class Model
      : public QObject
      , public elle::Printable
    {
    public:
      typedef uint32_t ID;
    protected:
      Model(fist::State& state,
            ID id);
    public:
      Model(Model const&) = default;
      virtual
      ~Model() = default;

    protected:
      fist::State& _state;
      ELLE_ATTRIBUTE_R(ID, id);

    private:
      /*----------.
      | Printable |
      `----------*/
      void
      print(std::ostream& stream) const override;

    private:
      Q_OBJECT;

    private:
      // Make State friend in order to allow it to send signals.
      friend fist::State;
    };
  }
}


#endif
