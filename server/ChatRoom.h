#pragma once

#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>
#include "ClientConnection.h"

// 聊天室单例类，管理用户连接及消息传递
class ChatRoom {
public:
    static ChatRoom& instance();    // 获取聊天室单例对象
    void join(const QString &user, QSharedPointer<ClientConnection> conn);  // 用户加入聊天室，保存对应连接
    void leave(const QString &user);    // 用户离开聊天室，移除对应连接
    void deliver(const QString &from, const QString &to, const QString &message);   // 发送聊天消息，从 from 到 to，传递消息内容
    void deliverFriendRequest(const QString &from, const QString &to);  // 发送好友申请通知，从 from 到 to
    void deliverFriendAccepted(const QString &from, const QString &to); // 发送好友同意通知，从 from 到 to
    void sendJsonToUser(const QString &user, const QJsonObject &obj);   // 新增接口，直接发送任意 JSON 对象给指定用户

private:
    ChatRoom() = default;                // 构造函数私有，保证单例
    QMutex mutex_;                      // 线程安全互斥锁，保护 sessions_ 数据
    QMap<QString, QSharedPointer<ClientConnection>> sessions_; // 用户连接映射表
};
