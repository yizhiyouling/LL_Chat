#pragma once

#include <QObject>
#include <QWebSocketServer>
#include <QList>
#include "UserManager.h"
#include "ClientConnection.h"

// 聊天服务器类，管理 WebSocket 连接和客户端列表
class ChatServer : public QObject {
    Q_OBJECT
public:
    explicit ChatServer(quint16 port, QObject *parent = nullptr);
    ~ChatServer();
    bool listen();  // 开始监听端口，返回是否成功

private slots:
    void onNewConnection(); // 新客户端连接处理槽函数
    void onClientClosed(ClientConnection *client);  // 客户端断开连接处理槽函数

private:
    QWebSocketServer *server_;                                 // WebSocket 服务器对象
    UserManager userMgr_;                                      // 用户管理器，处理用户数据和验证
    QList<QSharedPointer<ClientConnection>> clients_;          // 当前所有连接客户端列表
};
