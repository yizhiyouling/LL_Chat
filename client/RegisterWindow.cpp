#include "RegisterWindow.h"
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

// 构造函数，初始化注册窗口及UI组件，设置窗口标题和大小
RegisterWindow::RegisterWindow(WebSocketClient *client, QWidget *parent)
    : QWidget(parent),
    m_client(client)
{
    setWindowTitle("LL_Chat 注册");
    resize(300, 220);

    // 用户名标签和输入框
    QLabel *lblUser = new QLabel("用户名:", this);
    m_usernameEdit = new QLineEdit(this);

    // 密码标签和输入框，设置为密码模式
    QLabel *lblPass = new QLabel("密码:", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    // 确认密码标签和输入框，设置为密码模式
    QLabel *lblConfirm = new QLabel("确认密码:", this);
    m_confirmEdit = new QLineEdit(this);
    m_confirmEdit->setEchoMode(QLineEdit::Password);

    // 显示密码复选框，切换密码显示模式
    m_showPwdChk = new QCheckBox("显示密码", this);
    connect(m_showPwdChk, &QCheckBox::toggled, this, [this](bool on){
        m_passwordEdit->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
        m_confirmEdit->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
    });

    // 注册和取消按钮
    m_registerBtn = new QPushButton("注册", this);
    m_cancelBtn   = new QPushButton("取消", this);

    // 主布局，垂直排列所有控件
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(lblUser);
    mainLayout->addWidget(m_usernameEdit);
    mainLayout->addWidget(lblPass);
    mainLayout->addWidget(m_passwordEdit);
    mainLayout->addWidget(lblConfirm);
    mainLayout->addWidget(m_confirmEdit);
    mainLayout->addWidget(m_showPwdChk);

    // 按钮水平布局，放置注册和取消按钮
    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_registerBtn);
    btnLayout->addWidget(m_cancelBtn);
    mainLayout->addLayout(btnLayout);

    // 信号槽连接：注册按钮点击触发注册逻辑
    connect(m_registerBtn, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    // 取消按钮点击关闭窗口
    connect(m_cancelBtn, &QPushButton::clicked, this, &RegisterWindow::onCancelClicked);
    // 客户端接收服务器消息，处理注册结果
    connect(m_client, &WebSocketClient::messageReceived,
            this, &RegisterWindow::onServerMessage);
}

// 析构函数
RegisterWindow::~RegisterWindow() = default;

// 注册按钮点击事件处理
void RegisterWindow::onRegisterClicked()
{
    const QString u = m_usernameEdit->text().trimmed(); // 获取用户名，去除首尾空格
    const QString p = m_passwordEdit->text();            // 获取密码
    const QString c = m_confirmEdit->text();             // 获取确认密码

    // 检查用户名和密码是否为空
    if (u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空！");
        return;
    }
    // 检查两次密码输入是否一致
    if (p != c) {
        QMessageBox::warning(this, "提示", "两次输入的密码不一致，请重新输入！");
        m_passwordEdit->clear();
        m_confirmEdit->clear();
        return;
    }

    // 构造注册请求的JSON对象并发送
    QJsonObject req;
    req["type"]     = "register";
    req["username"] = u;
    req["password"] = p;
    m_client->sendJson(req);
}

// 取消按钮点击事件，关闭注册窗口
void RegisterWindow::onCancelClicked()
{
    close();
}

// 处理服务器返回的注册结果消息
void RegisterWindow::onServerMessage(const QJsonObject &obj)
{
    // 过滤非注册结果消息
    if (obj.value("type").toString() != "register_result")
        return;

    bool ok = obj.value("success").toBool(); // 注册是否成功
    if (ok) {
        // 注册成功提示，关闭注册窗口
        QMessageBox::information(this, "注册成功", "请返回登录界面进行登录。");
        close();
    } else {
        // 注册失败提示，显示原因
        const QString reason = obj.value("reason").toString("未知错误");
        QMessageBox::warning(this, "注册失败", reason);
    }
}
