#include "ClientConnection.h"
#include "ChatRoom.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

ClientConnection::ClientConnection(QWebSocket *socket, UserManager &mgr, QObject *parent)
    : QObject(parent), socket_(socket), userMgr_(mgr)
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
    if (!doc.isObject()) return;
    processMessage(doc.object());
}

void ClientConnection::onDisconnected()
{
    if (!username_.isEmpty())
        ChatRoom::instance().leave(username_);
    socket_->deleteLater();
    emit closed(this);
}

void ClientConnection::processMessage(const QJsonObject &obj)
{
    QString type = obj.value("type").toString();
    if (type == "register") {
        bool ok = userMgr_.registerUser(obj["username"].toString(),
                                        obj["password"].toString());
        QJsonObject resp{{"type","register_result"},
                         {"success", ok},
                         {"reason", ok ? "" : "用户名已存在"}};
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    else if (type == "login") {
        QString u = obj["username"].toString();
        bool ok = userMgr_.verifyLogin(u, obj["password"].toString());
        QJsonObject resp{{"type","login_result"},
                         {"success", ok},
                         {"reason", ok ? "" : "用户名或密码错误"}};
        if (ok) {
            username_ = u;
            ChatRoom::instance().join(username_, QSharedPointer<ClientConnection>(this));
        }
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    else if (type == "get_contacts") {
        QStringList list = userMgr_.getContacts(obj["username"].toString());
        QJsonObject resp{{"type","contacts_list"},
                         {"list", QJsonArray::fromStringList(list)}};
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    else if (type == "add_friend") {
        bool ok = userMgr_.addFriend(obj["from"].toString(),
                                     obj["to"].toString());
        QJsonObject resp{{"type","add_friend_result"},
                         {"success", ok},
                         {"reason", ok ? "" : "添加失败"}};
        sendText(QJsonDocument(resp).toJson(QJsonDocument::Compact));
    }
    else if (type == "chat") {
        ChatRoom::instance().deliver(
            obj["from"].toString(),
            obj["to"].toString(),
            obj["message"].toString()
            );
    }
}
