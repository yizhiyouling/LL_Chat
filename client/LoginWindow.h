#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "WebSocketClient.h"

class QPushButton;
class QLineEdit;

// LoginWindow: 登录界面，发送登录请求并处理服务器响应
class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(WebSocketClient *client, QWidget *parent = nullptr);
    ~LoginWindow() override;

private slots:
    void onLoginClicked();               // 点击“登录”按钮时调用
    void onRegisterClicked();            // 点击“注册”按钮时打开注册窗口
    void onServerMessage(const QJsonObject &obj);  // 处理服务器返回的 JSON 消息

private:
    WebSocketClient *m_client;  // WebSocket 客户端实例
    QLineEdit       *m_usernameEdit;  // 用户名输入框
    QLineEdit       *m_passwordEdit;  // 密码输入框
    QPushButton     *m_loginBtn;      // 登录按钮
    QPushButton     *m_registerBtn;   // 注册按钮
};

#endif
