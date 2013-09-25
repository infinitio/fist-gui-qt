#ifndef MODELS_HH
# define MODELS_HH

# include <QString>
# include <QPixmap>

struct User
{
public:
  User(QString name, QString const& avatar_file);
  User(QString name, QPixmap const avatar);

public:
  QString name;
  QPixmap const avatar;
};

struct Transaction
{
public:
  Transaction(QString filename, User const& user);

public:
  QString filename;
  User const& user;
};

#endif // !MODELS_HH
