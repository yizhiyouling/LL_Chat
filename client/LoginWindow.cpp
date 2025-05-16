#include "LoginWindow.h"
#include "ChatWindow.h"
#include "RegisterWindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

LoginWindow::LoginWindow(WebSocketClient *client, QWidget *parent)
    : QWidget(parent),
    m_client(client)
{
    setWindowTitle("LL_Chat 登录");
    resize(300, 180);

    QLabel *lblUser = new QLabel("用户名:", this);
    m_usernameEdit = new QLineEdit(this);
    QLabel *lblPass = new QLabel("密码:", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_loginBtn = new QPushButton("登录", this);
    m_registerBtn = new QPushButton("注册", this);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(lblUser);
    mainLayout->addWidget(m_usernameEdit);
    mainLayout->addWidget(lblPass);
    mainLayout->addWidget(m_passwordEdit);
    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_loginBtn);
    btnLayout->addWidget(m_registerBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(m_client, &WebSocketClient::messageReceived,
            this, &LoginWindow::onServerMessage);
}

LoginWindow::~LoginWindow() = default;

void LoginWindow::onLoginClicked()
{
    const QString u = m_usernameEdit->text().trimmed();
    const QString p = m_passwordEdit->text();
    if (u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空！");
        return;
    }
    QJsonObject req;
    req["type"] = "login";
    req["username"] = u;
    req["password"] = p;
    m_client->sendJson(req);
}

void LoginWindow::onRegisterClicked()
{
    RegisterWindow *reg = new RegisterWindow(m_client);
    reg->show();
}

void LoginWindow::onServerMessage(const QJsonObject &obj)
{
    if (obj.value("type").toString() != "login_result")
        return;

    bool ok = obj.value("success").toBool();
    if (ok) {
        ChatWindow *chat = new ChatWindow(m_usernameEdit->text().trimmed(), m_client);
        chat->show();
        close();
    } else {
        const QString reason = obj.value("reason").toString("未知错误");
        QMessageBox::warning(this, "登录失败", reason);
    }
}
