#pragma once

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include "UserManager.h"

class ClientConnection : public QObject {
    Q_OBJECT
public:
    explicit ClientConnection(QWebSocket *socket, UserManager &mgr, QObject *parent = nullptr);
    void sendText(const QByteArray &text);  // 发送文本消息给客户端

signals:
    void closed(ClientConnection*); // 客户端连接关闭时发出，传递自身指针

private slots:
    void onTextMessage(const QString &msg); // 收到文本消息时处理
    void onDisconnected();  // 客户端断开连接时处理

private:
    QWebSocket *socket_;     // 与客户端通信的 WebSocket 套接字
    UserManager &userMgr_;   // 用户管理器引用，用于身份验证等
    QString username_;       // 当前连接对应的用户名

    void processMessage(const QJsonObject &obj);    // 处理收到的 JSON 消息内容
};
