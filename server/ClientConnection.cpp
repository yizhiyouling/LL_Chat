#include "ClientConnection.h"
#include "ChatRoom.h"
#include "UserManager.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

ClientConnection::ClientConnection(QWebSocket *socket, UserManager &mgr, QObject *parent)
    : QObject(parent),
    socket_(socket),
    userMgr_(mgr)
{
    connect(socket_, &QWebSocket::textMessageReceived,
            this, &ClientConnection::onTextMessage);
    connect(socket_, &QWebSocket::disconnected,
            this, &ClientConnection::onDisconnected);
}

void ClientConnection::sendText(const QByteArray &text)
{
    socket_->sendTextMessage(QString::fromUtf8(text));
}

void ClientConnection::onTextMessage(const QString &msg)
{
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Received non-JSON message:" << msg;
        return;
    }
    processMessage(doc.object());
}

void ClientConnection::onDisconnected()
{
    if (!username_.isEmpty()) {
        ChatRoom::instance().leave(username_);
    }
    socket_->deleteLater();
    emit closed(this);
}

void ClientConnection::processMessage(const QJsonObject &obj)
{
    const QString type = obj.value("type").toString();

    // 注册
    if (type == "register") {
        const QString user = obj.value("username").toString();
        const QString pass = obj.value("password").toString();
        bool ok = userMgr_.registerUser(user, pass);

        QJsonObject resp {
            {"type",    "register_result"},
            {"success", ok},
            {"reason",  ok ? "" : "该用户名已存在"}
        };
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    // 登录
    else if (type == "login") {
        const QString user = obj.value("username").toString();
        const QString pass = obj.value("password").toString();
        bool ok = userMgr_.verifyLogin(user, pass);

        QJsonObject resp {
            {"type",    "login_result"},
            {"success", ok},
            {"reason",  ok ? "" : "用户名或密码错误"}
        };
        if (ok) {
            username_ = user;
            ChatRoom::instance().join(username_, QSharedPointer<ClientConnection>(this));
        }
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    // 获取联系人列表
    else if (type == "get_contacts") {
        const QString user = obj.value("username").toString();
        const QStringList list = userMgr_.getContacts(user);

        QJsonObject resp {
            {"type", "contacts_list"},
            {"list", QJsonArray::fromStringList(list)}
        };
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    // 发送好友申请
    else if (type == "add_friend") {
        const QString from = obj.value("from").toString();
        const QString to   = obj.value("to").toString();
        QString reason;
        bool ok = userMgr_.sendFriendRequest(from, to, reason);

        // 通知申请者
        QJsonObject resp {
            {"type",    "add_friend_result"},
            {"success", ok},
            {"reason",  reason}
        };
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));

        // 如果申请成功，通知被申请者
        if (ok) {
            ChatRoom::instance().deliverFriendRequest(from, to);
        }
    }
    // 获取所有待处理好友申请
    else if (type == "get_requests") {
        const QString user = obj.value("username").toString();
        const QStringList reqs = userMgr_.getFriendRequests(user);

        QJsonObject resp {
            {"type", "requests_list"},
            {"list", QJsonArray::fromStringList(reqs)}
        };
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    // 同意好友申请
    else if (type == "accept_friend") {
        const QString user = obj.value("user").toString();   // 接受者
        const QString from = obj.value("from").toString();   // 申请者
        QString reason;
        bool ok = userMgr_.acceptFriendRequest(user, from, reason);

        // 通知接受者结果
        QJsonObject respAccept{
            {"type",    "accept_friend_result"},
            {"success", ok},
            {"reason",  reason}
        };
        sendText(QJsonDocument(respAccept).toJson(QJsonDocument::Compact));

        if (ok) {
            // 通知申请者已被接受
            ChatRoom::instance().deliverFriendAccepted(user, from);
        }
        return;
    }
    // 聊天消息
    else if (type == "chat") {
        const QString from    = obj.value("from").toString();
        const QString to      = obj.value("to").toString();
        const QString message = obj.value("message").toString();
        ChatRoom::instance().deliver(from, to, message);
    }
}
