#include "UserManager.h"

bool UserManager::registerUser(const QString &user, const QString &pass)
{
    QMutexLocker lk(&mutex_);
    if (passwords_.contains(user)) return false;
    passwords_[user] = pass;
    contacts_[user] = QStringList();;
    friendRequests_[user] = QStringList();;
    return true;
}

bool UserManager::verifyLogin(const QString &user, const QString &pass)
{
    QMutexLocker lk(&mutex_);
    return passwords_.value(user) == pass;
}

bool UserManager::addFriendInternal(const QString &user, const QString &friendName)
{
    // 仅内部调用：已经做过检查
    auto &lst1 = contacts_[user];
    auto &lst2 = contacts_[friendName];
    if (!lst1.contains(friendName)) lst1.append(friendName);
    if (!lst2.contains(user))      lst2.append(user);
    return true;
}

bool UserManager::sendFriendRequest(const QString &from, const QString &to, QString &reason)
{
    QMutexLocker lk(&mutex_);
    if (from == to) {
        reason = "不能添加自己为好友";
        return false;
    }
    if (!passwords_.contains(to)) {
        reason = "对方用户不存在";
        return false;
    }
    if (contacts_[from].contains(to)) {
        reason = "已是好友";
        return false;
    }
    auto &reqs = friendRequests_[to];
    if (reqs.contains(from)) {
        reason = "已发送过申请";
        return false;
    }
    reqs.append(from);
    reason.clear();
    return true;
}

bool UserManager::acceptFriendRequest(const QString &user, const QString &from, QString &reason)
{
    QMutexLocker lk(&mutex_);
    auto &reqs = friendRequests_[user];
    if (!reqs.removeOne(from)) {
        reason = "无对应申请";
        return false;
    }
    // 双向添加好友
    addFriendInternal(user, from);
    reason.clear();
    return true;
}

QStringList UserManager::getContacts(const QString &user)
{
    QMutexLocker lk(&mutex_);
    return contacts_.value(user);
}

QStringList UserManager::getFriendRequests(const QString &user)
{
    QMutexLocker lk(&mutex_);
    return friendRequests_.value(user);
}
