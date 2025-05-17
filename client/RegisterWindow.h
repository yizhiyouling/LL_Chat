#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QCheckBox>
#include "WebSocketClient.h"

class QPushButton;
class QLineEdit;

// RegisterWindow: 注册界面，支持确认密码和显示密码功能
class RegisterWindow : public QWidget {
    Q_OBJECT

public:
    explicit RegisterWindow(WebSocketClient *client, QWidget *parent = nullptr);
    ~RegisterWindow() override;

private slots:
    void onRegisterClicked();          // 点击“注册”按钮时调用
    void onCancelClicked();            // 点击“取消”按钮时调用
    void onServerMessage(const QJsonObject &obj); // 处理服务器返回的注册结果

private:
    WebSocketClient *m_client;    // WebSocket 客户端实例
    QLineEdit       *m_usernameEdit;  // 用户名输入框
    QLineEdit       *m_passwordEdit;  // 密码输入框
    QLineEdit       *m_confirmEdit;   // 确认密码输入框
    QCheckBox       *m_showPwdChk;    // “显示密码”复选框
    QPushButton     *m_registerBtn;   // 注册按钮
    QPushButton     *m_cancelBtn;     // 取消按钮
};

#endif
