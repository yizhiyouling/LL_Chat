#pragma once

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include "UserManager.h"

class ClientConnection : public QObject {
    Q_OBJECT
public:
    explicit ClientConnection(QWebSocket *socket, UserManager &mgr, QObject *parent = nullptr);
    void sendText(const QByteArray &text);

signals:
    void closed(ClientConnection*);

private slots:
    void onTextMessage(const QString &msg);
    void onDisconnected();

private:
    QWebSocket *socket_;
    UserManager &userMgr_;
    QString username_;
    void processMessage(const QJsonObject &obj);
};
