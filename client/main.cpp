#include <QApplication>
#include <QUrl>
#include "WebSocketClient.h"
#include "ConnectionDialog.h"
#include "LoginWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 将你的阿里云服务器地址填在这里
    QUrl serverUrl(QStringLiteral("ws://localhost:9000"));
    WebSocketClient *client = new WebSocketClient;

    // 1. 弹出连接对话框，传入 serverUrl
    ConnectionDialog connDlg(client, serverUrl);
    if (connDlg.exec() != QDialog::Accepted)
        return 0;

    // 2. 连接成功后进入登录界面
    LoginWindow *login = new LoginWindow(client);
    login->show();

    return app.exec();
}
