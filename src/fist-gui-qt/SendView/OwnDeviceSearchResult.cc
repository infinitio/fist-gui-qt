# include <fist-gui-qt/SendView/OwnDeviceSearchResult.hh>
#include <fist-gui-qt/gui/IconProvider.hh>

namespace fist
{
  namespace sendview
  {
    OwnDeviceSearchResult::OwnDeviceSearchResult(
      fist::model::User const& model,
      fist::model::Device const& device,
      QWidget* parent)
      : SearchResultWidget(model, false, parent)
      , _device(device)
    {
      this->_avatar->set_avatar(gui::icon_provider().icons().at(device.type()));
      this->_fullname->setText(device.name());

      disconnect(&model, SIGNAL(avatar_updated()),
                 this, SLOT(_on_avatar_updated()));
    }

    void
    OwnDeviceSearchResult::_selected()
    {
      emit selected_device(this->_model.id(),
                           this->_device.id());
    }

    void
    OwnDeviceSearchResult::_unselected()
    {
      emit unselected_device(this->_model.id(),
                             this->_device.id());
    }
  }
}
