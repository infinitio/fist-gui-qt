#include <QByteArray>
#include <QBuffer>
#include <QImageReader>

#include <elle/log.hh>

#include <surface/gap/gap.hh>

#include <fist-gui-qt/State.hh>
#include <fist-gui-qt/model/User.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.model.User");

namespace fist
{
  namespace model
  {
    static
    surface::gap::User
    id_to_user(fist::State& state,
               uint32_t id)
    {
      surface::gap::User user;
      auto res = gap_user_by_id(state.state(), id, user);
      if (res != gap_ok); // XXX
      return user;
    }

    User::User(fist::State& state,
               uint32_t id)
      : User(state, id_to_user(state, id))
    {}

    User::User(fist::State& state,
               surface::gap::User const& user)
      : Super(state, user.id)
      , _user(user)
      , _avatar()
      , _default_avatar(true)
      , _new_avatar(true)
    {
    }

    QString
    User::fullname() const
    {
      return QString::fromUtf8(this->_user.fullname.c_str());
    }

    QString
    User::handle() const
    {
      return QString::fromUtf8(this->_user.handle.c_str());
    }

    void
    User::deleted(bool deleted)
    {
      this->_user.deleted = deleted;
    }

    bool
    User::deleted()
    {
      return this->_user.deleted;
    }

    bool
    User::swagger() const
    {
      return this->_user.swagger;
    }

    void
    User::swagger(bool)
    {
      this->_user.swagger = true;
    }

    void
    User::status(bool status)
    {
      this->_user.status = status;
      emit status_updated();
    }

    bool
    User::status() const
    {
      return this->_user.status;
    }

    bool
    User::new_avatar() const
    {
      return this->_new_avatar;
    }

    void
    User::avatar_available()
    {
      this->_new_avatar = true;
      emit avatar_updated();
    }

    QPixmap const&
    User::avatar() const
    {
      if (this->_avatar.isNull() || this->_default_avatar == true)
      {
        if (this->_new_avatar)
        {
          /// Get user icon data.
          void* data = nullptr;
          size_t len = 0;

          auto res = gap_avatar(this->_state.state(), this->id(), &data, &len);

          if (res == gap_ok)
          {
            if (len > 0) // An avatar is avalaible. If not, keep the default.
            {
              ELLE_DEBUG("%s: get avatar data", *this);
              QByteArray raw((char *) data, len);
              QBuffer buff(&raw);
              QImageReader reader;
              reader.setDecideFormatFromContent(true);
              reader.setDevice(&buff);
              this->_avatar =  QPixmap::fromImageReader(&reader);
            }
            else if(this->_avatar.isNull())
            {
              this->_avatar = QPixmap(QString(":/avatar_default"));
            }
            this->_default_avatar = false;
          }
          else if(this->_avatar.isNull())
          {
            ELLE_DEBUG("%s: avatar not available yet", *this);
            this->_avatar = QPixmap(QString(":/avatar_default"));
          }
        }
      }
      this->_new_avatar = false;
      return this->_avatar;
    }

    void
    User::print(std::ostream& stream) const
    {
      stream << "User(" << this->id() << ", " << this->_user.fullname << ")";
    }
  }
}
