#include <QCoreApplication>
#include "ChatServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    quint16 port = 9000;

    ChatServer server(port);
    if (!server.listen()) {
        qCritical() << "无法监听端口" << port;
        return 1;
    }

    qInfo() << "LL_Chat_Server_Qt 启动，监听 ws://localhost:" << port;
    return a.exec();
}
