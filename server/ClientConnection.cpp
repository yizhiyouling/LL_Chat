#include "ClientConnection.h"
#include "ChatRoom.h"
#include "UserManager.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

// 构造函数，初始化 WebSocket 连接并连接信号槽
ClientConnection::ClientConnection(QWebSocket *socket, UserManager &mgr, QObject *parent)
    : QObject(parent),
    socket_(socket),
    userMgr_(mgr)
{
    // 连接收到文本消息信号到处理函数
    connect(socket_, &QWebSocket::textMessageReceived,
            this, &ClientConnection::onTextMessage);
    // 连接断开信号到断开处理函数
    connect(socket_, &QWebSocket::disconnected,
            this, &ClientConnection::onDisconnected);
}

// 向客户端发送文本消息
void ClientConnection::sendText(const QByteArray &text)
{
    socket_->sendTextMessage(QString::fromUtf8(text));
}

// 处理收到的文本消息，解析成 JSON 并转给 processMessage 处理
void ClientConnection::onTextMessage(const QString &msg)
{
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Received non-JSON message:" << msg;
        return;
    }
    processMessage(doc.object());
}

// 处理连接断开事件，从聊天室移除用户并释放资源
void ClientConnection::onDisconnected()
{
    if (!username_.isEmpty()) {
        ChatRoom::instance().leave(username_);
    }
    socket_->deleteLater();
    emit closed(this);
}

// 根据消息类型处理不同业务逻辑，包括注册、登录、好友管理及聊天消息
void ClientConnection::processMessage(const QJsonObject &obj)
{
    const QString type = obj.value("type").toString();

    if (type == "register") {
        // 处理用户注册请求
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
    else if (type == "login") {
        // 处理用户登录请求
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
            // 用户成功登录后加入聊天室管理
            ChatRoom::instance().join(username_, QSharedPointer<ClientConnection>(this));
        }
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    else if (type == "get_contacts") {
        // 返回用户联系人列表
        const QString user = obj.value("username").toString();
        const QStringList list = userMgr_.getContacts(user);

        QJsonObject resp {
            {"type", "contacts_list"},
            {"list", QJsonArray::fromStringList(list)}
        };
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    else if (type == "add_friend") {
        // 处理发送好友申请
        const QString from = obj.value("from").toString();
        const QString to   = obj.value("to").toString();
        QString reason;
        bool ok = userMgr_.sendFriendRequest(from, to, reason);

        // 通知申请者申请结果
        QJsonObject resp {
            {"type",    "add_friend_result"},
            {"success", ok},
            {"reason",  reason}
        };
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));

        // 申请成功后通知被申请者
        if (ok) {
            ChatRoom::instance().deliverFriendRequest(from, to);
        }
    }
    else if (type == "get_requests") {
        // 获取用户所有待处理的好友申请列表
        const QString user = obj.value("username").toString();
        const QStringList reqs = userMgr_.getFriendRequests(user);

        QJsonObject resp {
            {"type", "requests_list"},
            {"list", QJsonArray::fromStringList(reqs)}
        };
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    else if (type == "accept_friend") {
        // 处理同意好友申请
        const QString user = obj.value("user").toString();
        const QString from = obj.value("from").toString();
        QString reason;
        bool ok = userMgr_.acceptFriendRequest(user, from, reason);

        // 通知同意者结果
        QJsonObject respAccept{
            {"type",    "accept_friend_result"},
            {"success", ok},
            {"reason",  reason}
        };
        sendText(QJsonDocument(respAccept).toJson(QJsonDocument::Compact));

        // 同意成功后通知申请者被接受
        if (ok) {
            ChatRoom::instance().deliverFriendAccepted(user, from);
        }
        return;
    }
    else if (type == "chat") {
        // 处理普通聊天消息转发
        const QString from    = obj.value("from").toString();
        const QString to      = obj.value("to").toString();
        const QString message = obj.value("message").toString();
        ChatRoom::instance().deliver(from, to, message);
    }
}
