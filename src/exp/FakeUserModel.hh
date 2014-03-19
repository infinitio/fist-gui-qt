#ifndef FAKEUSERMODEL_HH
# define FAKEUSERMODEL_HH

#include <fist-gui-qt/UserModel.hh>

class FakeUserModel:
  public UserModel
{
public:
  FakeUserModel(uint32_t id,
                QString const& fullname = "fulname",
                QString const& handle = "handle",
                UserModel::Transactions const& transactions = {}):
    UserModel(nullptr, id),
    _fakefullname(fullname),
    _fakehandle(handle),
    _faketransactions(transactions)
  {}

  QString const&
  fullname() const override
  {
    return this->_fakefullname;
  }

  QString const&
  handle() const override
  {
    return this->_fakehandle;
  }

  Transactions const&
  transactions() const override
  {
    return this->_faketransactions;
  }

  bool
  status() const override
  {
    return true;
  }

  QPixmap const&
  avatar() const
  {
    static QPixmap pix(QString(":/images/avatar_default.png"));
    return pix;
  }

  QString _fakefullname;
  QString _fakehandle;
  UserModel::Transactions _faketransactions;
};

#endif
