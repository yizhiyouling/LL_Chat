#include "RegisterWindow.h"
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

RegisterWindow::RegisterWindow(WebSocketClient *client, QWidget *parent)
    : QWidget(parent),
    m_client(client)
{
    setWindowTitle("LL_Chat 注册");
    resize(300, 220);

    QLabel *lblUser = new QLabel("用户名:", this);
    m_usernameEdit = new QLineEdit(this);

    QLabel *lblPass = new QLabel("密码:", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    QLabel *lblConfirm = new QLabel("确认密码:", this);
    m_confirmEdit = new QLineEdit(this);
    m_confirmEdit->setEchoMode(QLineEdit::Password);

    // 是否显示密码
    m_showPwdChk = new QCheckBox("显示密码", this);
    connect(m_showPwdChk, &QCheckBox::toggled, this, [this](bool on){
        m_passwordEdit->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
        m_confirmEdit->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
    });

    m_registerBtn = new QPushButton("注册", this);
    m_cancelBtn   = new QPushButton("取消", this);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(lblUser);
    mainLayout->addWidget(m_usernameEdit);
    mainLayout->addWidget(lblPass);
    mainLayout->addWidget(m_passwordEdit);
    mainLayout->addWidget(lblConfirm);
    mainLayout->addWidget(m_confirmEdit);
    mainLayout->addWidget(m_showPwdChk);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_registerBtn);
    btnLayout->addWidget(m_cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_registerBtn, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &RegisterWindow::onCancelClicked);
    connect(m_client, &WebSocketClient::messageReceived,
            this, &RegisterWindow::onServerMessage);
}

RegisterWindow::~RegisterWindow() = default;

void RegisterWindow::onRegisterClicked()
{
    const QString u = m_usernameEdit->text().trimmed();
    const QString p = m_passwordEdit->text();
    const QString c = m_confirmEdit->text();

    if (u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空！");
        return;
    }
    if (p != c) {
        QMessageBox::warning(this, "提示", "两次输入的密码不一致，请重新输入！");
        m_passwordEdit->clear();
        m_confirmEdit->clear();
        return;
    }

    QJsonObject req;
    req["type"]     = "register";
    req["username"] = u;
    req["password"] = p;
    m_client->sendJson(req);
}

void RegisterWindow::onCancelClicked()
{
    close();
}

void RegisterWindow::onServerMessage(const QJsonObject &obj)
{
    if (obj.value("type").toString() != "register_result")
        return;

    bool ok = obj.value("success").toBool();
    if (ok) {
        QMessageBox::information(this, "注册成功", "请返回登录界面进行登录。");
        close();
    } else {
        const QString reason = obj.value("reason").toString("未知错误");
        QMessageBox::warning(this, "注册失败", reason);
    }
}
