#pragma once

#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>
#include "ClientConnection.h"

class ChatRoom {
public:
    static ChatRoom& instance();

    void join(const QString &user, QSharedPointer<ClientConnection> conn);
    void leave(const QString &user);

    // 聊天消息
    void deliver(const QString &from, const QString &to, const QString &message);
    // 好友申请通知
    void deliverFriendRequest(const QString &from, const QString &to);
    // 好友同意通知
    void deliverFriendAccepted(const QString &from, const QString &to);

    // —— 新增：直接发送任意 JSON 对象 ——
    void sendJsonToUser(const QString &user, const QJsonObject &obj);

private:
    ChatRoom() = default;
    QMutex mutex_;
    QMap<QString, QSharedPointer<ClientConnection>> sessions_;
};
