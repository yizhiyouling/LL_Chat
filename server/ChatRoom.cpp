#include "ChatRoom.h"
#include <QJsonDocument>
#include <QJsonObject>

ChatRoom& ChatRoom::instance()
{
    static ChatRoom inst;
    return inst;
}

void ChatRoom::join(const QString &user, QSharedPointer<ClientConnection> conn)
{
    QMutexLocker lk(&mutex_);
    sessions_[user] = conn;
}

void ChatRoom::leave(const QString &user)
{
    QMutexLocker lk(&mutex_);
    sessions_.remove(user);
}

void ChatRoom::deliver(const QString &from, const QString &to, const QString &message)
{
    QMutexLocker lk(&mutex_);
    if (!sessions_.contains(to)) return;
    auto conn = sessions_.value(to);
    QJsonObject resp;
    resp["type"]    = "chat";
    resp["from"]    = from;
    resp["message"] = message;
    conn->sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
}

void ChatRoom::deliverFriendRequest(const QString &from, const QString &to)
{
    QMutexLocker lk(&mutex_);
    if (!sessions_.contains(to)) return;
    auto conn = sessions_.value(to);
    QJsonObject resp;
    resp["type"] = "friend_request";
    resp["from"] = from;
    conn->sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
}

void ChatRoom::deliverFriendAccepted(const QString &from, const QString &to)
{
    QMutexLocker lk(&mutex_);
    if (!sessions_.contains(to)) return;
    auto conn = sessions_.value(to);
    QJsonObject resp;
    resp["type"] = "friend_accepted";
    resp["from"] = from;
    conn->sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
}

// —— 新增方法实现 ——
void ChatRoom::sendJsonToUser(const QString &user, const QJsonObject &obj)
{
    QMutexLocker lk(&mutex_);
    if (!sessions_.contains(user)) return;
    auto conn = sessions_.value(user);
    conn->sendText(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}
