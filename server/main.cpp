#include <QCoreApplication>
#include "ChatServer.h"

// 主程序入口，启动聊天服务器监听指定端口
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    quint16 port = 9000; // 服务器监听端口

    ChatServer server(port);
    if (!server.listen()) {
        qCritical() << "无法监听端口" << port;
        return 1; // 监听失败退出
    }

    qInfo() << "LL_Chat_Server_Qt 启动，监听 ws://localhost:" << port;
    return a.exec(); // 进入 Qt 事件循环
}
