#include "WebSocketClient.h"
#include <QJsonDocument>
#include <QDebug>

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent)
{
    // 绑定底层信号
    connect(&m_webSocket, &QWebSocket::connected,
            this, &WebSocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);
    connect(&m_webSocket,
            &QWebSocket::errorOccurred,
            this,
            &WebSocketClient::errorOccurred);
}

void WebSocketClient::connectToServer(const QUrl &url)
{
    m_url = url;
    qDebug() << "Connecting to" << url.toString();
    m_webSocket.open(url);
}

void WebSocketClient::sendJson(const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson(QJsonDocument::Compact);
    m_webSocket.sendTextMessage(QString::fromUtf8(bytes));
}

void WebSocketClient::onConnected()
{
    qDebug() << "WebSocket connected";
    emit connected();
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Received non-JSON message:" << message;
        return;
    }
    emit messageReceived(doc.object());
}
