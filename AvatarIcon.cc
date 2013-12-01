#include <fist-gui-qt/AvatarIcon.hh>

AvatarIcon::AvatarIcon(QPixmap const& pixmap):
  _uid(0),
  _pixmap(QSize(30, 30)),
  _mask(QSize(30, 30)),
  _geometry(QPoint(0, 0), QSize(30, 30))
{
  QSize size(30, 30);
  this->setMinimumSize(size);
  this->setMaximumSize(size);
  this->_pixmap.fill(Qt::transparent);

  {
    _mask.fill(Qt::transparent);
    QPainter painter(&_mask);
    painter.setRenderHints(QPainter::Antialiasing |
                            QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(_geometry);
  }
  QPainter painter(&this->_pixmap);
  painter.setRenderHints(QPainter::Antialiasing |
                          QPainter::SmoothPixmapTransform);
  painter.drawPixmap(_geometry, pixmap);
  painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
  painter.drawPixmap(_geometry, _mask);
}

AvatarIcon::AvatarIcon(gap_State* state, uint32_t uid):
  _uid(uid),
  _pixmap(30, 30),
  _mask(30, 30),
  _geometry(QPoint(0, 0), QSize(30, 30))
{
  QSize size(30, 30);
  this->setMinimumSize(size);
  this->setMaximumSize(size);

  this->_pixmap.fill(Qt::transparent);
  {
    _mask.fill(Qt::transparent);
    QPainter painter(&_mask);
    painter.setRenderHints(QPainter::Antialiasing |
                            QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(_geometry);
  }

  QPainter painter(&this->_pixmap);
  painter.setRenderHints(QPainter::Antialiasing |
                          QPainter::SmoothPixmapTransform);

  // Look for the avatar in the avatars already loaded.
  auto avatar = g_avatars.find(uid);

  if (avatar == g_avatars.end())
  {
    // Start requesting the avatar.
    QNetworkAccessManager* m_netwManager = new QNetworkAccessManager(this);
    connect(m_netwManager,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(slot_netwManagerFinished(QNetworkReply*)));

    // Setup request string.
    const char* protocol = "http://";
    const char* url = gap_user_avatar_url(state, uid);
    std::string stdurl = std::string(protocol) + std::string(url);
    gap_free_user_avatar_url(url);

    // Request the image at address stdurl.
    QUrl qurl(stdurl.c_str());
    QNetworkRequest request(qurl);
    m_netwManager->get(request);
  }
  else
  {
    painter.drawPixmap(_geometry, g_avatars[uid]);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.drawPixmap(_geometry, _mask);
  }
}

void
AvatarIcon::slot_netwManagerFinished(QNetworkReply *reply)
{
  QPainter painter(&this->_pixmap);
  painter.setRenderHints(QPainter::Antialiasing |
                          QPainter::SmoothPixmapTransform);

  if (reply->error() == QNetworkReply::NoError)
  {
    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);

    painter.drawPixmap(_geometry, pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.drawPixmap(_geometry, _mask);

    g_avatars.insert(std::pair<uint32_t, QPixmap>(_uid, pixmap));
  }
  else
  {
    painter.drawPixmap(_geometry, QPixmap(QString("resources/avatar1.png")));
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.drawPixmap(_geometry, _mask);
  }
}

void
AvatarIcon::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing |
                          QPainter::SmoothPixmapTransform);
  painter.drawPixmap(_geometry, this->_pixmap);
}
