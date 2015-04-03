#ifndef FIST_GUI_QT_MODEL_DEVICE_HH
# define FIST_GUI_QT_MODEL_DEVICE_HH

# include <QObject>
# include <QString>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <surface/gap/Device.hh>

# include <fist-gui-qt/gui/IconProvider.hh>
# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace model
  {
    class Device
      : public elle::Printable
    {
    public:
      Device(surface::gap::Device const& device);

    private:
      ELLE_ATTRIBUTE_R(QString, id);
      ELLE_ATTRIBUTE_R(QString, name);
      ELLE_ATTRIBUTE_R(QString, os);
      ELLE_ATTRIBUTE_R(gui::IconProvider::Type, type);

    private:
      /*----------.
      | Printable |
      `----------*/
      void
      print(std::ostream& stream) const override;
    };
  }
}

# endif