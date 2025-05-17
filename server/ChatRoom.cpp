#include "ChatRoom.h"
#include <QJsonDocument>
#include <QJsonObject>

// 获取聊天室单例实例
ChatRoom& ChatRoom::instance()
{
    static ChatRoom inst;
    return inst;
}

// 用户加入聊天室，保存其连接对象，线程安全
void ChatRoom::join(const QString &user, QSharedPointer<ClientConnection> conn)
{
    QMutexLocker lk(&mutex_);
    sessions_[user] = conn;
}

// 用户离开聊天室，移除对应连接，线程安全
void ChatRoom::leave(const QString &user)
{
    QMutexLocker lk(&mutex_);
    sessions_.remove(user);
}

// 发送聊天消息给指定用户，封装成 JSON 格式，线程安全
void ChatRoom::deliver(const QString &from, const QString &to, const QString &message)
{
    QMutexLocker lk(&mutex_);
    if (!sessions_.contains(to)) return;
    auto conn = sessions_.value(to);
    QJsonObject resp;
    resp["type"]    = "chat";      // 消息类型
    resp["from"]    = from;        // 发送者
    resp["message"] = message;     // 消息内容
    conn->sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
}

// 发送好友申请通知给指定用户，线程安全
void ChatRoom::deliverFriendRequest(const QString &from, const QString &to)
{
    QMutexLocker lk(&mutex_);
    if (!sessions_.contains(to)) return;
    auto conn = sessions_.value(to);
    QJsonObject resp;
    resp["type"] = "friend_request"; // 类型为好友申请
    resp["from"] = from;             // 申请者
    conn->sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
}

// 发送好友同意通知给指定用户，线程安全
void ChatRoom::deliverFriendAccepted(const QString &from, const QString &to)
{
    QMutexLocker lk(&mutex_);
    if (!sessions_.contains(to)) return;
    auto conn = sessions_.value(to);
    QJsonObject resp;
    resp["type"] = "friend_accepted"; // 类型为好友同意
    resp["from"] = from;              // 同意者
    conn->sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
}

// 发送任意 JSON 对象给指定用户，线程安全
void ChatRoom::sendJsonToUser(const QString &user, const QJsonObject &obj)
{
    QMutexLocker lk(&mutex_);
    if (!sessions_.contains(user)) return;
    auto conn = sessions_.value(user);
    conn->sendText(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}
