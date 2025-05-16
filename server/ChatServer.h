#pragma once

#include <QObject>
#include <QWebSocketServer>
#include <QList>
#include "UserManager.h"
#include "ClientConnection.h"

class ChatServer : public QObject {
    Q_OBJECT
public:
    explicit ChatServer(quint16 port, QObject *parent = nullptr);
    bool listen();

private slots:
    void onNewConnection();
    void onClientClosed(ClientConnection *client);

private:
    QWebSocketServer *server_;
    UserManager userMgr_;
    QList<QSharedPointer<ClientConnection>> clients_;
};
