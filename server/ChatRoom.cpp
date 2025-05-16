#include "ChatRoom.h"
#include <QJsonObject>
#include <QJsonDocument>

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
