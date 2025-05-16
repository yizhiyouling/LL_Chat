#pragma once

#include <QString>
#include <QMap>
#include <QStringList>
#include <QMutex>

class UserManager {
public:
    bool registerUser(const QString &user, const QString &pass);
    bool verifyLogin(const QString &user, const QString &pass);
    bool addFriend(const QString &user, const QString &friendName);
    QStringList getContacts(const QString &user);

private:
    QMutex mutex_;
    QMap<QString, QString> passwords_;
    QMap<QString, QStringList> contacts_;
};
