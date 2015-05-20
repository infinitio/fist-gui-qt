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
      auto empty_user = [&user] {
        ELLE_WARN("got an empty user id");
        user.id = gap_null();
        user.status = false;
        user.fullname = "Unknown User";
        user.handle = "Unknown User";
        user.meta_id = ""; // Risky.
        user.swagger = false;
        user.deleted = true;
        user.ghost = false;
        user.phone_number = "";
        user.ghost_code = "";
        user.ghost_invitation_url = "";
      };
      if (id != gap_null())
      {
        auto res = gap_user_by_id(state.state(), id, user);
        if (res != gap_ok)
          empty_user();
      }
      else
        empty_user();
      return user;
    }

    User::User(fist::State& state,
               uint32_t id)
      : User(state, id_to_user(state, id))
    {}

    User::User(fist::State& state,
               surface::gap::User const& user)
      : Super(state, user.id)
      , _model(user)
      , _avatar()
      , _default_avatar(true)
      , _new_avatar(true)
    {
    }

    void
    User::model(surface::gap::User const& user)
    {
      auto status = this->_model.status;
      this->_model = user;
      if (status != this->_model.status)
        emit status_updated();
    }

    bool
    User::me() const
    {
      return this->id() == this->_state.me().id();
    }

    QString
    User::fullname() const
    {
      return QString::fromUtf8(this->_model.fullname.c_str());
    }

    QString
    User::handle() const
    {
      return QString::fromUtf8(this->_model.handle.c_str());
    }

    void
    User::deleted(bool deleted)
    {
      this->_model.deleted = deleted;
    }

    bool
    User::deleted()
    {
      return this->_model.deleted;
    }

    bool
    User::swagger() const
    {
      return this->_model.swagger;
    }

    void
    User::swagger(bool)
    {
      this->_model.swagger = true;
    }

    bool
    User::ghost() const
    {
      return this->_model.ghost;
    }

    void
    User::status(bool status)
    {
      if (status != this->_model.status)
      {
        this->_model.status = status;
        emit status_updated();
      }
    }

    bool
    User::status() const
    {
      return this->_model.status;
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
      static const QPixmap default_avatar(QString(":/avatar_default"));
      if (this->_new_avatar)
      {
        auto& array = this->_state.avatar(this->id());
        if (array.size() != 0)
        {
          QBuffer buffer(&array);
          QImageReader reader;
          reader.setDecideFormatFromContent(true);
          reader.setDevice(&buffer);
          this->_avatar =  QPixmap::fromImageReader(&reader);
        }
        else
        {
          this->_avatar = default_avatar;
        }
        this->_new_avatar = false;
      }
      return this->_avatar;
    }

    void
    User::print(std::ostream& stream) const
    {
      stream << "User(" << this->id() << ", " << this->_model.fullname << ")";
    }
  }
}
