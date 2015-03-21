#ifndef FIST_GUI_QT_SEND_VIEW_OWNDEVICESEARCHRESULT_HH
# define FIST_GUI_QT_SEND_VIEW_OWNDEVICESEARCHRESULT_HH

# include <fist-gui-qt/SendView/SearchResultWidget.hh>

namespace fist
{
  namespace sendview
  {
    class OwnDeviceSearchResult
      : public SearchResultWidget
    {
    public:
      OwnDeviceSearchResult(fist::model::User const& model,
                            fist::model::Device const& device,
                            QWidget* parent = nullptr);

      ELLE_ATTRIBUTE_R(fist::model::Device, device);

    signals:
      void
      selected_device(uint32_t id,
                      QString const& device_id);

      void
      unselected_device(uint32_t id,
                        QString const& device_id);

    private slots:
      void
      _selected() override;

      void
      _unselected() override;

    private:
      Q_OBJECT;
    };
  }
}


#endif
