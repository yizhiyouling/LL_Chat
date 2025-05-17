#include "UserManager.h"

// 注册新用户，用户名唯一，成功返回true
bool UserManager::registerUser(const QString &user, const QString &pass)
{
    QMutexLocker lk(&mutex_);
    if (passwords_.contains(user)) return false;  // 用户已存在，注册失败
    passwords_[user] = pass;                      // 保存密码
    contacts_[user] = QStringList();              // 初始化好友列表为空
    friendRequests_[user] = QStringList();        // 初始化好友申请列表为空
    return true;
}

// 验证用户名和密码，成功返回true
bool UserManager::verifyLogin(const QString &user, const QString &pass)
{
    QMutexLocker lk(&mutex_);
    return passwords_.value(user) == pass;        // 判断密码是否匹配
}

// 内部调用，双向添加好友，返回true
bool UserManager::addFriendInternal(const QString &user, const QString &friendName)
{
    // 仅内部调用：调用前已做参数合法性检查
    auto &lst1 = contacts_[user];
    auto &lst2 = contacts_[friendName];
    if (!lst1.contains(friendName)) lst1.append(friendName);
    if (!lst2.contains(user))      lst2.append(user);
    return true;
}

// 发送好友申请，成功返回true，并通过reason说明失败原因
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
    reqs.append(from);  // 添加好友申请
    reason.clear();
    return true;
}

// 同意好友申请，成功返回true，并通过reason说明失败原因
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

// 获取指定用户的好友列表
QStringList UserManager::getContacts(const QString &user)
{
    QMutexLocker lk(&mutex_);
    return contacts_.value(user);
}

// 获取指定用户所有待处理的好友申请列表
QStringList UserManager::getFriendRequests(const QString &user)
{
    QMutexLocker lk(&mutex_);
    return friendRequests_.value(user);
}
