#pragma once

#include <QString>
#include <QMap>
#include <QStringList>
#include <QMutex>

class UserManager {
public:
    // 注册新用户，返回是否成功（用户名唯一）
    bool registerUser(const QString &user, const QString &pass);

    // 验证用户名和密码是否匹配，返回是否成功登录
    bool verifyLogin(const QString &user, const QString &pass);

    // 内部添加好友关系，返回是否成功
    bool addFriendInternal(const QString &user, const QString &friendName);

    // 发送好友申请，成功返回true
    bool sendFriendRequest(const QString &from, const QString &to, QString &reason);

    // 同意好友申请，成功返回true
    bool acceptFriendRequest(const QString &user, const QString &from, QString &reason);

    // 获取某用户的好友列表
    QStringList getContacts(const QString &user);

    // 获取某用户所有待处理的好友申请列表
    QStringList getFriendRequests(const QString &user);

private:
    QMutex mutex_; // 保护数据的互斥锁
    QMap<QString, QString>      passwords_;       // 用户密码存储，key为用户名，value为密码
    QMap<QString, QStringList>  contacts_;        // 用户好友列表，key为用户名，value为好友用户名列表
    QMap<QString, QStringList>  friendRequests_;  // 待处理好友申请，key为用户名，value为申请者用户名列表
};
