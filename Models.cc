#include "Models.hh"

User::User(QString name, QPixmap const avatar):
  name(name),
  avatar(avatar)
{}

User::User(QString name, QString const& avatar_file):
  User(name, QPixmap(avatar_file))
{}

Transaction::Transaction(QString filename, User const& user):
  filename(filename),
  user(user)
{}
