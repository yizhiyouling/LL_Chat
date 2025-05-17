#include "WebSocketClient.h"
#include <QJsonDocument>
#include <QDebug>

// 构造函数，初始化 WebSocket 并连接相关信号槽
WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent)
{
    // 连接 WebSocket 连接成功信号到对应槽函数
    connect(&m_webSocket, &QWebSocket::connected,
            this, &WebSocketClient::onConnected);
    // 连接接收到文本消息信号到对应槽函数
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);
    // 连接 WebSocket 错误信号，直接转发
    connect(&m_webSocket,
            &QWebSocket::errorOccurred,
            this,
            &WebSocketClient::errorOccurred);
}

// 连接到服务器，保存 URL 并打开连接
void WebSocketClient::connectToServer(const QUrl &url)
{
    m_url = url;
    qDebug() << "Connecting to" << url.toString();
    m_webSocket.open(url);
}

// 发送 JSON 对象，转换为字符串并通过 WebSocket 发送
void WebSocketClient::sendJson(const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson(QJsonDocument::Compact);
    m_webSocket.sendTextMessage(QString::fromUtf8(bytes));
}

// WebSocket 连接成功槽函数，发出 connected 信号
void WebSocketClient::onConnected()
{
    qDebug() << "WebSocket connected";
    emit connected();
}

// 收到文本消息，尝试解析为 JSON 对象，解析成功则发射 messageReceived 信号
void WebSocketClient::onTextMessageReceived(const QString &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Received non-JSON message:" << message;
        return;
    }
    emit messageReceived(doc.object());
}
