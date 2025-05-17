#include "LoginWindow.h"
#include "ChatWindow.h"
#include "RegisterWindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

// 构造函数：初始化登录界面布局和信号槽
LoginWindow::LoginWindow(WebSocketClient *client, QWidget *parent)
    : QWidget(parent),
    m_client(client)
{
    // 窗口标题和大小
    setWindowTitle("LL_Chat 登录");
    resize(300, 180);

    // 用户名和密码标签与输入框
    QLabel *lblUser = new QLabel("用户名:", this);
    m_usernameEdit = new QLineEdit(this);
    QLabel *lblPass = new QLabel("密码:", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);  // 设置为密码模式

    // 登录和注册按钮
    m_loginBtn    = new QPushButton("登录", this);
    m_registerBtn = new QPushButton("注册", this);

    // 垂直布局：用户名、密码及按钮
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(lblUser);
    mainLayout->addWidget(m_usernameEdit);
    mainLayout->addWidget(lblPass);
    mainLayout->addWidget(m_passwordEdit);

    // 水平布局：登录和注册按钮并列
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_loginBtn);
    btnLayout->addWidget(m_registerBtn);
    mainLayout->addLayout(btnLayout);

    // 连接按钮点击和消息接收到对应槽函数
    connect(m_loginBtn,    &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(m_client,      &WebSocketClient::messageReceived,
            this,          &LoginWindow::onServerMessage);
}

// 析构函数
LoginWindow::~LoginWindow() = default;

// 点击“登录”按钮时调用：本地验证后发送登录请求
void LoginWindow::onLoginClicked()
{
    // 获取并修剪用户输入
    const QString u = m_usernameEdit->text().trimmed();
    const QString p = m_passwordEdit->text();
    // 用户名或密码为空时提示
    if (u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空！");
        return;
    }
    // 构造登录 JSON 请求并发送
    QJsonObject req;
    req["type"]     = "login";
    req["username"] = u;
    req["password"] = p;
    m_client->sendJson(req);
}

// 点击“注册”按钮时调用：打开注册窗口
void LoginWindow::onRegisterClicked()
{
    RegisterWindow *reg = new RegisterWindow(m_client);
    reg->show();
}

// 处理服务器返回的消息
void LoginWindow::onServerMessage(const QJsonObject &obj)
{
    // 仅处理 login_result 类型
    if (obj.value("type").toString() != "login_result")
        return;

    // 根据 success 字段判断登录结果
    bool ok = obj.value("success").toBool();
    if (ok) {
        // 登录成功，打开聊天主界面并关闭登录界面
        ChatWindow *chat = new ChatWindow(m_usernameEdit->text().trimmed(), m_client);
        chat->show();
        close();
    } else {
        // 登录失败时显示原因
        const QString reason = obj.value("reason").toString("未知错误");
        QMessageBox::warning(this, "登录失败", reason);
    }
}
