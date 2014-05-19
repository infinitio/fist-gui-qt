#include <QString>
#include <QStringList>

#include <fist-gui-qt/fwd.hh>
#include <fist-gui-qt/model/Link.hh>

namespace fist
{
  namespace model
  {
    QString
    name_from_url(QUrl const& url)
    {
      return url.toString().split("/").last();
    }

    // QUrl::fromPercentEncoding(
    Link::Link(fist::State& state,
               uint32_t id)
      : Super(state, id)
      , _url("https://scontent-a-lga.xx.fbcdn.net/hphotos-frc3/t1.0-9/p417x417/10246470_10152867841193957_2463857070587152167_n.jpg")
      , _name(name_from_url(_url))
      , _mtime(QDateTime::currentDateTime())
      , _download_count(7)
      , _progress(0.6)
    {}

    void
    Link::print(std::ostream& stream) const
    {
      stream << "Link(" << this->id() << ")";
    }
  }
}
