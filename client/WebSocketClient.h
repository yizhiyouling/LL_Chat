#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QJsonObject>
#include <QUrl>
#include <QAbstractSocket>

class WebSocketClient : public QObject {
    Q_OBJECT
public:
    explicit WebSocketClient(QObject *parent = nullptr);

    // 连接到服务器
    void connectToServer(const QUrl &url);
    // 获取当前连接 URL（用于重试）
    QUrl serverUrl() const { return m_url; }
    // 发送 JSON 对象
    void sendJson(const QJsonObject &obj);

signals:
    // 收到服务器消息时发出
    void messageReceived(const QJsonObject &obj);
    // WebSocket 已连接
    void connected();
    // WebSocket 发生错误
    void errorOccurred(QAbstractSocket::SocketError error);

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);

private:
    QWebSocket m_webSocket;
    QUrl       m_url;
};

#endif // W
