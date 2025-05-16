#pragma once

#include <QString>
#include <QMap>
#include <QSharedPointer>
#include "ClientConnection.h"

class ChatRoom {
public:
    static ChatRoom& instance();

    void join(const QString &user, QSharedPointer<ClientConnection> conn);
    void leave(const QString &user);
    void deliver(const QString &from, const QString &to, const QString &message);

private:
    ChatRoom() = default;
    QMutex mutex_;
    QMap<QString, QSharedPointer<ClientConnection>> sessions_;
};
