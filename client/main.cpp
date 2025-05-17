#include <QApplication>
#include <QUrl>
#include "WebSocketClient.h"
#include "ConnectionDialog.h"
#include "LoginWindow.h"

// main: 程序入口，初始化 QApplication 并启动客户端流程
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 指定服务器 WebSocket 地址
    QUrl serverUrl(QStringLiteral("ws://localhost:9000"));

    // 创建 WebSocketClient 实例，用于网络通信
    WebSocketClient *client = new WebSocketClient;

    // 弹出连接对话框，尝试连接服务器
    ConnectionDialog connDlg(client, serverUrl);
    // 如果用户取消或连接失败且不重试，则退出程序
    if (connDlg.exec() != QDialog::Accepted)
        return 0;

    // 连接成功后，弹出登录窗口
    LoginWindow *login = new LoginWindow(client);
    login->show();

    // 进入 Qt 事件循环
    return app.exec();
}
