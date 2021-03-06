#ifndef FIST_PANEL_HH
# define FIST_PANEL_HH

# include <QVBoxLayout>
# include <QSystemTrayIcon>

# include <elle/Printable.hh>

# include <fist-gui-qt/SmoothLayout.hh>
# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/SystrayMessage.hh>

class Panel
  : public SmoothLayout
{
  typedef SmoothLayout Super;

public:
  Panel(Footer* footer,
        QWidget* owner = nullptr);

protected:
  SmoothLayout* _body;
  Footer* _footer;

  void
  childEvent(QChildEvent*) override;

private:
  virtual
  void
  _on_show() {};

  virtual
  void
  _on_hide() {};

public:

  void
  showEvent(QShowEvent* event) override;

  void
  hideEvent(QHideEvent* event) override;

Q_SIGNALS:
  void
  set_background_color(QColor const& color);

  void
  shown();
public:
  QSize
  minimumSizeHint() const override;

public:
  virtual
  Footer*
  footer();

private:
  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT;
};

#endif
