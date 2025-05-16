#pragma once

#include <QString>
#include <QMap>
#include <QStringList>
#include <QMutex>

class UserManager {
public:
    bool registerUser(const QString &user, const QString &pass);
    bool verifyLogin(const QString &user, const QString &pass);

    // 原有好友存储
    bool addFriendInternal(const QString &user, const QString &friendName);

    // ① 发送好友申请
    bool sendFriendRequest(const QString &from, const QString &to, QString &reason);
    // ② 同意好友申请
    bool acceptFriendRequest(const QString &user, const QString &from, QString &reason);

    QStringList getContacts(const QString &user);
    // ③ 获取某人所有待处理申请
    QStringList getFriendRequests(const QString &user);

private:
    QMutex mutex_;
    QMap<QString, QString>      passwords_;
    QMap<QString, QStringList>  contacts_;
    QMap<QString, QStringList>  friendRequests_;  // 待处理申请
};
