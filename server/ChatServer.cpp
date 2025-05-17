#include "ChatServer.h"
#include <QDebug>

ChatServer::ChatServer(quint16 port, QObject *parent)
    : QObject(parent)
{
    server_ = new QWebSocketServer(QStringLiteral("LL_Chat_Server"),
                                   QWebSocketServer::NonSecureMode,
                                   this);
    server_->listen(QHostAddress::Any, port);
    connect(server_, &QWebSocketServer::newConnection,
            this, &ChatServer::onNewConnection);
}

ChatServer::~ChatServer() {
}

bool ChatServer::listen()
{
    return server_->isListening();
}

void ChatServer::onNewConnection()
{
    QWebSocket *sock = server_->nextPendingConnection();
    auto conn = QSharedPointer<ClientConnection>::create(sock, userMgr_, this);
    clients_.append(conn);
    connect(conn.data(), &ClientConnection::closed,
            this, &ChatServer::onClientClosed);
}

void ChatServer::onClientClosed(ClientConnection *client)
{
    // 删除已关闭客户端
    for (int i = 0; i < clients_.size(); ++i) {
        if (clients_[i].data() == client) {
            clients_.removeAt(i);
            break;
        }
    }
}
