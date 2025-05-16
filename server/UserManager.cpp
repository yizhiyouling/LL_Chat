#include "UserManager.h"

bool UserManager::registerUser(const QString &user, const QString &pass)
{
    QMutexLocker lk(&mutex_);
    if (passwords_.contains(user)) return false;
    passwords_[user] = pass;
    contacts_[user] = {};
    return true;
}

bool UserManager::verifyLogin(const QString &user, const QString &pass)
{
    QMutexLocker lk(&mutex_);
    return passwords_.value(user) == pass;
}

bool UserManager::addFriend(const QString &user, const QString &friendName)
{
    QMutexLocker lk(&mutex_);
    if (!passwords_.contains(friendName)) return false;
    auto &lst = contacts_[user];
    if (lst.contains(friendName)) return false;
    lst.append(friendName);
    contacts_[friendName].append(user);
    return true;
}

QStringList UserManager::getContacts(const QString &user)
{
    QMutexLocker lk(&mutex_);
    return contacts_.value(user);
}
