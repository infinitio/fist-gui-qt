# include <fist-gui-qt/SendView/OwnDeviceSearchResult.hh>
#include <fist-gui-qt/gui/IconProvider.hh>

namespace fist
{
  namespace sendview
  {
    OwnDeviceSearchResult::OwnDeviceSearchResult(fist::model::User const& user,
                                                 fist::model::Device const& device,
                                                 bool selected,
                                                 QWidget* parent)
      : Super(selected, parent)
      , _device(device)
      , _id(user.id())
    {
      this->_avatar->set_avatar(gui::icon_provider().icons().at(device.type()));
      this->_fullname->setText(device.name());
    }

    void
    OwnDeviceSearchResult::_selected()
    {
      emit selected_device(this->id(), this->_device.id());
    }

    void
    OwnDeviceSearchResult::_unselected()
    {
      emit unselected_device(this->id(), this->_device.id());
    }
  }
}
