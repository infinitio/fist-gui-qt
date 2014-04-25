#ifndef LOGINFOOTER_HH
# define LOGINFOOTER_HH

# include <fist-gui-qt/Footer.hh>

class LoginFooter:
  public Footer
{
public:
  LoginFooter(QWidget* parent = nullptr);

  virtual
  ~LoginFooter() = default;

public slots:
  void
  click();

Q_SIGNALS:
  void
  released();

private:
  void
  mouseReleaseEvent(QMouseEvent * event) override;

  void
  enterEvent(QEvent* event) override;

  void
  leaveEvent(QEvent* event) override;

private:
  Q_OBJECT
};

#endif
