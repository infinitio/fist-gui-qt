#ifndef FAKETRANSACTIONMODEL_HH
# define FAKETRANSACTIONMODEL_HH

# include <QTime>

# include <surface/gap/gap.h>

# include <fist-gui-qt/TransactionModel.hh>

class FakeTransactionModel:
  public TransactionModel
{
public:
  FakeTransactionModel(uint32_t id,
                       bool is_sender = true,
                       QVector<QString> const& files = {"foo.txt"},
                       QString const& message = "",
                       gap_TransactionStatus status = gap_transaction_running):
    TransactionModel(nullptr, id),
    _fakeis_sender(is_sender),
    _fakefiles(files),
    _fakemessage(message),
    _fakestatus(status)
  {}

  bool
  is_sender() const override
  {
    return this->_fakeis_sender;
  }

  QVector<QString> const&
  files() const override
  {
    return this->_fakefiles;
  }

  gap_TransactionStatus
  status() const override
  {
    return this->_fakestatus;
  }

  QString const&
  message() const override
  {
    return this->_fakemessage;
  }

  QDateTime const&
  mtime() const override
  {
    QDateTime date;
    return date;
  }

  float
  progress() const override
  {
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    static float progress = (qrand() % 100) * 0.01f;
    return progress;
  }

  bool _fakeis_sender;
  QString _fakemessage;
  QVector<QString> _fakefiles;
  gap_TransactionStatus _fakestatus;
};


#endif
