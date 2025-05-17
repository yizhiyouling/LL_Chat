#include "ChatWindow.h"
#include "AddFriendDialog.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

// 构造函数：初始化界面、布局、信号槽，并拉取联系人列表
ChatWindow::ChatWindow(const QString &username, WebSocketClient *client, QWidget *parent)
    : QWidget(parent),
    m_username(username),
    m_client(client)
{
    // 设置窗口标题和大小
    setWindowTitle(QString("LL_Chat — %1").arg(username));
    resize(600, 400);

    // 创建联系人列表、聊天视图、输入框和按钮
    m_contactList  = new QListWidget(this);
    m_chatView     = new QListWidget(this);
    m_messageEdit  = new QLineEdit(this);
    m_sendBtn      = new QPushButton("发送", this);
    m_addFriendBtn = new QPushButton("添加好友", this);

    // 布局：左右分栏 + 底部输入
    auto *mainLayout  = new QHBoxLayout(this);
    auto *leftLayout  = new QVBoxLayout();
    auto *rightLayout = new QVBoxLayout();
    auto *inputLayout = new QHBoxLayout();

    // 左侧：联系人列表和添加好友按钮
    leftLayout->addWidget(new QLabel("联系人："));
    leftLayout->addWidget(m_contactList);
    leftLayout->addWidget(m_addFriendBtn);

    // 右侧：聊天记录和发送区域
    rightLayout->addWidget(new QLabel("聊天记录："));
    rightLayout->addWidget(m_chatView);
    inputLayout->addWidget(m_messageEdit);
    inputLayout->addWidget(m_sendBtn);
    rightLayout->addLayout(inputLayout);

    // 将左右布局添加到主布局
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 2);
    setLayout(mainLayout);

    // 信号－槽连接
    connect(m_sendBtn, &QPushButton::clicked, this, &ChatWindow::onSendClicked);
    connect(m_addFriendBtn, &QPushButton::clicked, this, &ChatWindow::onAddFriendClicked);
    connect(m_contactList, &QListWidget::itemClicked, this, &ChatWindow::onContactSelected);
    connect(m_client, &WebSocketClient::messageReceived, this, &ChatWindow::onServerMessage);

    // 启动时向服务端请求联系人列表
    QJsonObject reqContacts{{"type", "get_contacts"}, {"username", m_username}};
    m_client->sendJson(reqContacts);
}

// 析构函数
ChatWindow::~ChatWindow() = default;

// 点击“发送”按钮时调用：发送消息并更新本地历史
void ChatWindow::onSendClicked()
{
    // 获取当前选中的联系人
    QListWidgetItem *item = m_contactList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "提示", "请先选择联系人！");
        return;
    }

    QString to  = item->text();
    QString msg = m_messageEdit->text().trimmed();
    if (msg.isEmpty()) {
        // 空消息不发送
        return;
    }

    // 构造 chat JSON 并发送给服务器
    QJsonObject reqChat{
        {"type",    "chat"},
        {"from",    m_username},
        {"to",      to},
        {"message", msg}
    };
    m_client->sendJson(reqChat);

    // 将消息添加到本地历史并刷新当前视图
    m_history[to].append(QString("我 -> %1: %2").arg(to, msg));
    showHistoryFor(to);

    // 清空输入框
    m_messageEdit->clear();
}

// 点击“添加好友”按钮时弹出对话框
void ChatWindow::onAddFriendClicked()
{
    AddFriendDialog dlg(m_username, m_client, this);
    dlg.exec();
}

// 选中联系人时展示对应聊天历史
void ChatWindow::onContactSelected(QListWidgetItem *item)
{
    if (!item) {
        return;
    }
    showHistoryFor(item->text());
}

// 显示指定联系人的聊天历史
void ChatWindow::showHistoryFor(const QString &contact)
{
    m_chatView->clear();
    for (const QString &line : m_history.value(contact)) {
        m_chatView->addItem(line);
    }
}

// 处理从服务器接收的各类消息
void ChatWindow::onServerMessage(const QJsonObject &obj)
{
    QString type = obj.value("type").toString();

    if (type == "chat") {
        // 收到聊天消息
        QString from = obj.value("from").toString();
        QString msg  = obj.value("message").toString();
        QString line = QString("%1: %2").arg(from, msg);

        // 存入本地历史，若当前聊天对应发送者则刷新视图
        m_history[from].append(line);
        QListWidgetItem *cur = m_contactList->currentItem();
        if (cur && cur->text() == from) {
            showHistoryFor(from);
        }
    }
    else if (type == "contacts_list") {
        // 更新联系人列表
        m_contactList->clear();
        for (const auto &v : obj.value("list").toArray()) {
            m_contactList->addItem(v.toString());
        }
    }
    else if (type == "friend_request") {
        // 收到新的好友申请通知，弹出消息并刷新申请列表
        QString from = obj.value("from").toString();
        QMessageBox::information(this, "好友请求",
                                 QString("用户 %1 请求添加你为好友！").arg(from));
    }
    else if (type == "add_friend_result") {
        // 收到发起好友申请的结果
        bool ok = obj.value("success").toBool();
        QString reason = obj.value("reason").toString();
        QMessageBox::information(this,
                                 ok ? "申请已发送" : "申请失败",
                                 ok ? "好友申请已发送" : reason);
    }
    else if (type == "accept_friend_result") {
        // 收到同意好友申请的结果（接受者端）
        bool ok = obj.value("success").toBool();
        QString reason = obj.value("reason").toString();
        if (ok) {
            QMessageBox::information(this, "已成为好友", "你已同意好友请求！");
            // 拉取最新联系人列表
            QJsonObject req{{"type", "get_contacts"}, {"username", m_username}};
            m_client->sendJson(req);
        } else {
            QMessageBox::warning(this, "同意失败", reason);
        }
    }
    else if (type == "friend_accepted") {
        // 收到对方同意好友申请的通知（申请者端）
        QString from = obj.value("from").toString();
        QMessageBox::information(this, "好友通过",
                                 QString("用户 %1 接受了你的好友申请！").arg(from));
        // 拉取最新联系人列表
        QJsonObject req{{"type", "get_contacts"}, {"username", m_username}};
        m_client->sendJson(req);
    }
}
