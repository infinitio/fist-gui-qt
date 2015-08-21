#ifndef FIST_GUI_QT_SENDVIEW_NOMORESTORAGEWINDOW_HH
# define FIST_GUI_QT_SENDVIEW_NOMORESTORAGEWINDOW_HH

# include <QMainWindow>
# include <QPushButton>

# include <elle/attribute.hh>

# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace popup
  {
    class UpgradePlan
      : public QMainWindow
    {
      typedef QMainWindow Super;
    public:
      UpgradePlan(QString const& title,
                  QString const& text,
                  fist::State const& state,
                  QWidget* parent = nullptr,
                  QString const& cancel_button_test = "",
                  bool show_invite = true);

    private slots:
      void
      _go_to_website();

    private:
      void
      showEvent(QShowEvent* event) override;

      void
      paintEvent(QPaintEvent* event) override;

      ELLE_ATTRIBUTE(QPushButton*, upgrade);
      ELLE_ATTRIBUTE(fist::State const&, state);
    protected:
      virtual
      QString const&
      _campaign() const = 0;

    private:
      Q_OBJECT;
    };

    class NoMoreStorage
      : public UpgradePlan
    {
    public:
      NoMoreStorage(fist::State& state,
                    QWidget* parent);

    protected:
      QString const&
      _campaign() const override;
    };

   class SendToSelfQuotaExceeded
      : public UpgradePlan
    {
    public:
      SendToSelfQuotaExceeded(fist::State& state,
                              QWidget* parent);

    protected:
      QString const&
      _campaign() const override;
    };

   class GhostDownloadsLimit
      : public UpgradePlan
    {
    public:
      GhostDownloadsLimit(fist::State& state,
                          QString const& recipient,
                          QWidget* parent);

    protected:
      QString const&
      _campaign() const override;
    };

  }
}
#endif
