#include "ChatServer.h"
#include <QDebug>

// 构造函数，创建 WebSocket 服务器并监听指定端口
ChatServer::ChatServer(quint16 port, QObject *parent)
    : QObject(parent)
{
    // 创建非加密 WebSocket 服务器实例
    server_ = new QWebSocketServer(QStringLiteral("LL_Chat_Server"),
                                   QWebSocketServer::NonSecureMode,
                                   this);
    // 开始监听所有网络接口指定端口
    server_->listen(QHostAddress::Any, port);
    // 连接新连接信号到槽函数
    connect(server_, &QWebSocketServer::newConnection,
            this, &ChatServer::onNewConnection);
}

// 析构函数
ChatServer::~ChatServer() {
}

// 返回服务器是否正在监听
bool ChatServer::listen()
{
    return server_->isListening();
}

// 新客户端连接处理
void ChatServer::onNewConnection()
{
    // 获取下一个待处理的客户端套接字
    QWebSocket *sock = server_->nextPendingConnection();
    // 创建客户端连接对象，传入套接字和用户管理器
    auto conn = QSharedPointer<ClientConnection>::create(sock, userMgr_, this);
    // 添加到客户端列表
    clients_.append(conn);
    // 连接客户端关闭信号到服务器槽函数，方便移除连接
    connect(conn.data(), &ClientConnection::closed,
            this, &ChatServer::onClientClosed);
}

// 客户端关闭时调用，移除该客户端连接
void ChatServer::onClientClosed(ClientConnection *client)
{
    // 遍历客户端列表，查找匹配的连接并删除
    for (int i = 0; i < clients_.size(); ++i) {
        if (clients_[i].data() == client) {
            clients_.removeAt(i);
            break;
        }
    }
}
