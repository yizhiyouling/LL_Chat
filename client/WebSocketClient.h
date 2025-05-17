#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QJsonObject>
#include <QUrl>
#include <QAbstractSocket>

// WebSocket 客户端类，负责与服务器建立连接并收发消息
class WebSocketClient : public QObject {
    Q_OBJECT
public:
    explicit WebSocketClient(QObject *parent = nullptr);
    void connectToServer(const QUrl &url);  // 连接到指定服务器 URL
    QUrl serverUrl() const { return m_url; }     // 获取当前连接的服务器 URL（用于重试连接等场景）
    void sendJson(const QJsonObject &obj);  // 发送 JSON 格式数据给服务器

signals:
    void messageReceived(const QJsonObject &obj);    // 收到服务器消息时发出，传递解析后的 JSON 对象
    void connected();   // WebSocket 连接成功信号
    void errorOccurred(QAbstractSocket::SocketError error); // 发生网络错误时发出，传递错误类型

private slots:
    void onConnected(); // WebSocket 连接成功回调
    void onTextMessageReceived(const QString &message); // 接收到文本消息回调

private:
    QWebSocket m_webSocket; // WebSocket 实例
    QUrl       m_url;       // 服务器 URL
};

#endif
