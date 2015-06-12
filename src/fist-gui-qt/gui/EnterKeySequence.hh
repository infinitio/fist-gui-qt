#ifndef FIST_GUI_QT_GUI_ENTERKEYSEQUENCE_HH
# define FIST_GUI_QT_GUI_ENTERKEYSEQUENCE_HH

# include <QLineEdit>
# include <QKeySequence>

# include <elle/attribute.hh>

namespace fist
{
  namespace gui
  {
    class EnterKeySequence
      : public QLineEdit
    {
    public:
      EnterKeySequence(QString const& text,
                       QWidget *parent = 0);

    protected:
      void
      keyPressEvent(QKeyEvent *e) override;

    public:
      ELLE_ATTRIBUTE_R(QKeySequence, previous);
      ELLE_ATTRIBUTE_R(QKeySequence, sequence);

    signals:
      void
      no_modifier();

    public slots:
      void
      rollback();

    private:
      Q_OBJECT;
    };
  }
}

#endif
